#include "State/Player/PlayerStateMachine.h"
#include "State/Player/PlayerIdleState.h"
#include "State/Player/PlayerWalkState.h"
#include "State/Player/PlayerDashState.h"
#include "State/Player/PlayerJumpState.h"
#include "State/Player/PlayerCrouchState.h"
#include "State/Player/PlayerDeadState.h"
#include "State/Player/PlayerShadowState.h"
#include "State/Player/PlayerInteractState.h"
#include "State/Player/PlayerLocomotionState.h"
#include "State/Player/PlayerStateBase.h"
#include "State/Player/PlayerDamageState.h"
#include "State/Player/PlayerAttackState.h"
#include "State/Player/PlayerInteractAnimationState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerStateMachine::UPlayerStateMachine()
{
	InitialStateClass = UPlayerIdleState::StaticClass();
}

void UPlayerStateMachine::Initialize(AActor* Owner)
{
	Super::Initialize(Owner);
	bConfigured = false;
	const APlayerCharacter* Player = Cast<APlayerCharacter>(Owner);
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStateMachine requires a PlayerCharacter owner."));
		return;
	}
	StaminaUsed = 0.f;
	CancelLocomotionRequests();
	//入力で使うステートの登録確認
	bool bValid = true;
	const TSubclassOf<UPlayerStateBase> RequiredStates[] = {
		UPlayerIdleState::StaticClass(),
		UPlayerWalkState::StaticClass(),
		UPlayerDashState::StaticClass(),
		UPlayerJumpState::StaticClass(),
		UPlayerCrouchState::StaticClass(),
		UPlayerAttackState::StaticClass(),
		UPlayerDamageState::StaticClass(),
		UPlayerDeadState::StaticClass(),
		UPlayerShadowState::StaticClass(),
		UPlayerInteractState::StaticClass(),
		UPlayerInteractAnimationState::StaticClass()
	};
	for (const auto& StateClass : RequiredStates)
	{
		int32 Count = 0;
		for (UStateBase* State : GetRegisteredStates())
		{
			if (IsValid(State) && State->IsA(StateClass)) { ++Count; }
		}
		if (Count != 1)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Register exactly one subclass of %s."), *GetName(), *StateClass->GetName());
			bValid = false;
		}
	}
	for (UStateBase* State : GetRegisteredStates())
	{
		if (!IsValid(Cast<UPlayerStateBase>(State))) { bValid = false; }
	}
	bConfigured = bValid;
	if (bConfigured && !ChangePlayerState(InitialStateClass))
	{
		bConfigured = false;
		UE_LOG(LogTemp, Error, TEXT("%s: Initial player state refused entry."), *GetName());
	}
}

bool UPlayerStateMachine::ChangePlayerState(TSubclassOf<UPlayerStateBase> NewStateClass)
{
	if (!bConfigured || bTransitionInProgress) { return false; }
	TGuardValue<bool> TransitionGuard(bTransitionInProgress, true);
	UPlayerStateBase* State = GetPlayerState(NewStateClass);
	if (!State || IsCurrentState<UPlayerDeadState>()) { return false; }
	//被ダメージと死亡は終了待ち中でも割り込む
	if (State->IsA<UPlayerDamageState>() || State->IsA<UPlayerDeadState>())
	{
		return ForceChangeState(State);
	}
	return ChangeState(State);
}

bool UPlayerStateMachine::IsLocomotionState() const
{
	return bConfigured && IsCurrentState<UPlayerLocomotionState>();
}

void UPlayerStateMachine::Stop()
{
	if (bTransitionInProgress) { return; }
	TGuardValue<bool> TransitionGuard(bTransitionInProgress, true);
	Super::Stop();
	if (UPlayerDamageState* DamageState = GetState<UPlayerDamageState>()) { DamageState->ClearProtection(); }
	CancelLocomotionRequests();
	bConfigured = false;
}

UPlayerStateBase* UPlayerStateMachine::GetPlayerState(TSubclassOf<UPlayerStateBase> StateClass) const
{
	if (!StateClass) { return nullptr; }
	for (UStateBase* State : GetRegisteredStates())
	{
		if (IsValid(State) && State->IsA(StateClass)) { return Cast<UPlayerStateBase>(State); }
	}
	return nullptr;
}

bool UPlayerStateMachine::CanControl() const
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	const UPlayerStateBase* State = Cast<UPlayerStateBase>(GetCurrentState());
	return bConfigured && IsValid(PlayerCharacter) && PlayerCharacter->m_bCanControl
		&& State && State->AllowsManualControl();
}

void UPlayerStateMachine::Update(float DeltaTime)
{
	if (!bConfigured) { return; }
	DeltaTime = FMath::Max(0.f, DeltaTime);
	for (UStateBase* RegisteredState : GetRegisteredStates())
	{
		if (UPlayerStateBase* State = Cast<UPlayerStateBase>(RegisteredState)) { State->BackgroundUpdate(DeltaTime); }
	}
	Super::Update(DeltaTime);
	UPlayerStateBase* State = Cast<UPlayerStateBase>(GetCurrentState());
	if (!IsValid(State)) { return; }
	const float Rate = FMath::Max(0.f, State->GetStaminaConsumptionRate());
	if (Rate > 0.f)
	{
		StaminaUsed = FMath::Clamp(StaminaUsed + DeltaTime * Rate, 0.f, GetMaximumStamina());
		if (IsStaminaDepleted()) { State->OnStaminaDepleted(); }
	}
	else { StaminaUsed = FMath::Max(0.f, StaminaUsed - DeltaTime * FMath::Max(0.f, StaminaRecoveryRate)); }
}

TSubclassOf<UPlayerStateBase> UPlayerStateMachine::SelectLocomotionState(bool bAfterLanding) const
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!IsValid(PlayerCharacter)) { return UPlayerIdleState::StaticClass(); }
	const UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement();
	if (!bAfterLanding && IsValid(Movement) && Movement->IsFalling()) { return UPlayerJumpState::StaticClass(); }
	if (bCrouchRequested) { return UPlayerCrouchState::StaticClass(); }
	if (bDashRequested && !IsStaminaDepleted() && !PlayerCharacter->m_charaMoveInput.IsNearlyZero(0.1f)) { return UPlayerDashState::StaticClass(); }
	return PlayerCharacter->m_charaMoveInput.IsNearlyZero(0.1f) ? UPlayerIdleState::StaticClass() : UPlayerWalkState::StaticClass();
}

void UPlayerStateMachine::RefreshLocomotionState()
{
	if (IsLocomotionState() && !IsCurrentState<UPlayerJumpState>()) { ChangePlayerState(SelectLocomotionState()); }
}

bool UPlayerStateMachine::ReturnToLocomotion(bool bAfterLanding)
{
	return ChangePlayerState(SelectLocomotionState(bAfterLanding));
}

void UPlayerStateMachine::MoveInput()
{
	if (!CanControl()) { return; }
	RefreshLocomotionState();
	if (UPlayerStateBase* State = Cast<UPlayerStateBase>(GetCurrentState())) { State->OnMoveInput(); }
}

void UPlayerStateMachine::SetDashRequested(bool bRequested)
{
	if (bRequested && (!CanControl() || !IsLocomotionState() || bCrouchRequested || IsStaminaDepleted())) { return; }
	bDashRequested = bRequested;
	RefreshLocomotionState();
}

void UPlayerStateMachine::ToggleCrouch()
{
	if (!CanControl() || !IsLocomotionState()) { return; }
	bCrouchRequested = !bCrouchRequested;
	RefreshLocomotionState();
}

void UPlayerStateMachine::TryJump()
{
	if (!CanControl() || !IsLocomotionState() || IsCurrentState<UPlayerJumpState>()) { return; }
	bJumpRequested = true;
	ChangePlayerState(UPlayerJumpState::StaticClass());
	bJumpRequested = false;
}

void UPlayerStateMachine::TryAttack()
{
	if (!CanControl()) { return; }
	if (IsCurrentState<UPlayerShadowState>() && !ReturnToLocomotion()) { return; }
	ChangePlayerState(UPlayerAttackState::StaticClass());
}

void UPlayerStateMachine::ToggleShadow()
{
	if (!CanControl()) { return; }
	if (IsCurrentState<UPlayerShadowState>()) { ReturnToLocomotion(); }
	else { ChangePlayerState(UPlayerShadowState::StaticClass()); }
}

void UPlayerStateMachine::TryInteract()
{
	if (CanControl() && IsLocomotionState() && !IsCurrentState<UPlayerJumpState>()) { ChangePlayerState(UPlayerInteractState::StaticClass()); }
}

void UPlayerStateMachine::NotifyLanded()
{
	if (IsCurrentState<UPlayerJumpState>()) { ReturnToLocomotion(true); }
}

void UPlayerStateMachine::NotifyAttackFinished()
{
	if (!IsCurrentState<UPlayerAttackState>()) { return; }
	if (UPlayerAttackState* State = GetState<UPlayerAttackState>()) { State->NotifyAttackFinished(); }
}

void UPlayerStateMachine::NotifyInteractionFinished()
{
	if (!IsCurrentState<UPlayerInteractAnimationState>()) { return; }
	if (UPlayerInteractAnimationState* State = GetState<UPlayerInteractAnimationState>()) { State->CompleteInteraction(); }
}

void UPlayerStateMachine::ReceiveDamage(int32 Amount, const FVector& Knockback)
{
	if (UPlayerDamageState* State = GetState<UPlayerDamageState>()) { State->ReceiveDamage(Amount, Knockback); }
}

bool UPlayerStateMachine::IsInvincible() const
{
	const UPlayerDamageState* State = GetState<UPlayerDamageState>();
	return State && State->IsInvincible();
}

bool UPlayerStateMachine::IsAttacking() const
{
	const UPlayerAttackState* State = GetState<UPlayerAttackState>();
	return IsCurrentState<UPlayerAttackState>() && State && State->IsAttackActive();
}
