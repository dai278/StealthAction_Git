#include "State/Player/PlayerStateMachine.h"
#include "State/Player/PlayerStateBase.h"
#include "State/Player/PlayerDamageState.h"
#include "State/Player/PlayerAttackState.h"
#include "State/Player/PlayerInteractAnimationState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

void UPlayerStateMachine::Initialize(AActor* Owner)
{
	Super::Initialize(Owner);
	PlayerStates.Reset();
	bConfigured = false;
	const APlayerCharacter* Player = Cast<APlayerCharacter>(Owner);
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStateMachine requires a PlayerCharacter owner."));
		return;
	}
	StaminaUsed = 0.f;
	CancelLocomotionRequests();
	bool bValid = true;
	for (UStateBase* State : GetRegisteredStates())
	{
		UPlayerStateBase* PlayerState = Cast<UPlayerStateBase>(State);
		if (!PlayerState || PlayerStates.Contains(PlayerState->GetPlayerStatus()))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: State Classes must contain unique player statuses and only PlayerStateBase subclasses."), *GetName());
			bValid = false;
			continue;
		}
		PlayerStates.Add(PlayerState->GetPlayerStatus(), PlayerState);
	}

	// Existing character inputs rely on all of these statuses. Fail configuration
	// explicitly rather than silently applying half a transition's side effects.
	const EPlayerStatus RequiredStatuses[] = {
		EPlayerStatus::Idle, EPlayerStatus::Walk, EPlayerStatus::Dash,
		EPlayerStatus::Jump, EPlayerStatus::Crouch, EPlayerStatus::Attack,
		EPlayerStatus::Damage, EPlayerStatus::Dead, EPlayerStatus::InShadow,
		EPlayerStatus::Interact, EPlayerStatus::InteractAnimation
	};
	for (EPlayerStatus Status : RequiredStatuses)
	{
		if (!PlayerStates.Contains(Status))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Missing State Classes entry for EPlayerStatus %d."), *GetName(), static_cast<int32>(Status));
			bValid = false;
		}
	}
	bConfigured = bValid;
	if (bConfigured && !ChangePlayerState(InitialStatus))
	{
		bConfigured = false;
		UE_LOG(LogTemp, Error, TEXT("%s: Initial player state refused entry."), *GetName());
	}
}

bool UPlayerStateMachine::ChangePlayerState(EPlayerStatus NewStatus)
{
	if (!bConfigured || bTransitionInProgress) { return false; }
	TGuardValue<bool> TransitionGuard(bTransitionInProgress, true);
	const TObjectPtr<UPlayerStateBase>* State = PlayerStates.Find(NewStatus);
	if (!State || !IsValid(State->Get())) { return false; }
	const UPlayerStateBase* Current = Cast<UPlayerStateBase>(GetCurrentState());
	if (Current && Current->GetPlayerStatus() == EPlayerStatus::Dead) { return false; }
	// Damage and death must interrupt attack/interaction even if a BP locks exit.
	if (NewStatus == EPlayerStatus::Damage || NewStatus == EPlayerStatus::Dead)
	{
		return ForceChangeState(State->Get());
	}
	return ChangeState(State->Get());
}

EPlayerStatus UPlayerStateMachine::GetPlayerStatus() const
{
	const UPlayerStateBase* State = Cast<UPlayerStateBase>(GetCurrentState());
	return State ? State->GetPlayerStatus() : InitialStatus;
}

bool UPlayerStateMachine::IsLocomotionState() const
{
	if (!bConfigured || !GetCurrentState()) { return false; }
	switch (GetPlayerStatus())
	{
	case EPlayerStatus::Idle:
	case EPlayerStatus::Walk:
	case EPlayerStatus::Dash:
	case EPlayerStatus::Jump:
	case EPlayerStatus::Crouch:
		return true;
	default:
		return false;
	}
}

void UPlayerStateMachine::Stop()
{
	if (bTransitionInProgress) { return; }
	TGuardValue<bool> TransitionGuard(bTransitionInProgress, true);
	Super::Stop();
	if (UPlayerDamageState* DamageState = Cast<UPlayerDamageState>(GetPlayerState(EPlayerStatus::Damage))) { DamageState->ClearProtection(); }
	CancelLocomotionRequests();
	bConfigured = false;
}

UPlayerStateBase* UPlayerStateMachine::GetPlayerState(EPlayerStatus Status) const
{
	const TObjectPtr<UPlayerStateBase>* State = PlayerStates.Find(Status);
	return State && IsValid(State->Get()) ? State->Get() : nullptr;
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
	for (const auto& Entry : PlayerStates)
	{
		if (IsValid(Entry.Value)) { Entry.Value->BackgroundUpdate(DeltaTime); }
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

EPlayerStatus UPlayerStateMachine::SelectLocomotionStatus(bool bAfterLanding) const
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!IsValid(PlayerCharacter)) { return EPlayerStatus::Idle; }
	const UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement();
	if (!bAfterLanding && IsValid(Movement) && Movement->IsFalling()) { return EPlayerStatus::Jump; }
	if (bCrouchRequested) { return EPlayerStatus::Crouch; }
	if (bDashRequested && !IsStaminaDepleted() && !PlayerCharacter->m_charaMoveInput.IsNearlyZero(0.1f)) { return EPlayerStatus::Dash; }
	return PlayerCharacter->m_charaMoveInput.IsNearlyZero(0.1f) ? EPlayerStatus::Idle : EPlayerStatus::Walk;
}

void UPlayerStateMachine::RefreshLocomotionState()
{
	if (IsLocomotionState() && GetPlayerStatus() != EPlayerStatus::Jump) { ChangePlayerState(SelectLocomotionStatus()); }
}

bool UPlayerStateMachine::ReturnToLocomotion(bool bAfterLanding)
{
	return ChangePlayerState(SelectLocomotionStatus(bAfterLanding));
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
	if (!CanControl() || !IsLocomotionState() || GetPlayerStatus() == EPlayerStatus::Jump) { return; }
	bJumpRequested = true;
	ChangePlayerState(EPlayerStatus::Jump);
	bJumpRequested = false;
}

void UPlayerStateMachine::TryAttack()
{
	if (!CanControl()) { return; }
	if (GetPlayerStatus() == EPlayerStatus::InShadow && !ReturnToLocomotion()) { return; }
	ChangePlayerState(EPlayerStatus::Attack);
}

void UPlayerStateMachine::ToggleShadow()
{
	if (!CanControl()) { return; }
	if (GetPlayerStatus() == EPlayerStatus::InShadow) { ReturnToLocomotion(); }
	else { ChangePlayerState(EPlayerStatus::InShadow); }
}

void UPlayerStateMachine::TryInteract()
{
	if (CanControl() && IsLocomotionState() && GetPlayerStatus() != EPlayerStatus::Jump) { ChangePlayerState(EPlayerStatus::Interact); }
}

void UPlayerStateMachine::NotifyLanded()
{
	if (GetPlayerStatus() == EPlayerStatus::Jump) { ReturnToLocomotion(true); }
}

void UPlayerStateMachine::NotifyAttackFinished()
{
	if (GetPlayerStatus() != EPlayerStatus::Attack) { return; }
	if (UPlayerAttackState* State = Cast<UPlayerAttackState>(GetPlayerState(EPlayerStatus::Attack))) { State->NotifyAttackFinished(); }
}

void UPlayerStateMachine::NotifyInteractionFinished()
{
	if (GetPlayerStatus() != EPlayerStatus::InteractAnimation) { return; }
	if (UPlayerInteractAnimationState* State = Cast<UPlayerInteractAnimationState>(GetPlayerState(EPlayerStatus::InteractAnimation))) { State->CompleteInteraction(); }
}

void UPlayerStateMachine::ReceiveDamage(int32 Amount, const FVector& Knockback)
{
	if (UPlayerDamageState* State = Cast<UPlayerDamageState>(GetPlayerState(EPlayerStatus::Damage))) { State->ReceiveDamage(Amount, Knockback); }
}

bool UPlayerStateMachine::IsInvincible() const
{
	const UPlayerDamageState* State = Cast<UPlayerDamageState>(GetPlayerState(EPlayerStatus::Damage));
	return State && State->IsInvincible();
}

bool UPlayerStateMachine::IsAttacking() const
{
	const UPlayerAttackState* State = Cast<UPlayerAttackState>(GetPlayerState(EPlayerStatus::Attack));
	return GetPlayerStatus() == EPlayerStatus::Attack && State && State->IsAttackActive();
}
