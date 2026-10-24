//----------------------------------------------------------
//概要：プレイヤーのステート管理
//----------------------------------------------------------

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
#include "State/Player/PlayerNormalAttackState.h"
#include "State/Player/PlayerInteractAnimationState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerStateMachine::UPlayerStateMachine()
{
	InitialStateClass = UPlayerIdleState::StaticClass();
}

//-----------------------------------------------------
//初期化
//-----------------------------------------------------
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
	const TSubclassOf<UPlayerStateBase> RequiredStates[] = {UPlayerIdleState::StaticClass(),
															UPlayerWalkState::StaticClass(),
															UPlayerDashState::StaticClass(),
															UPlayerJumpState::StaticClass(),
															UPlayerCrouchState::StaticClass(),
															UPlayerAttackState::StaticClass(),
															UPlayerDamageState::StaticClass(),
															UPlayerDeadState::StaticClass(),
															UPlayerShadowState::StaticClass(),
															UPlayerInteractState::StaticClass(),
															UPlayerInteractAnimationState::StaticClass()};
	for (const auto& StateClass : RequiredStates)
	{
		int32 Count = 0;
		for (UStateBase* State : GetRegisteredStates())
		{
			if (IsValid(State) && State->IsA(StateClass))
			{
				++Count;
			}
		}
		if (Count != 1)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Register exactly one subclass of %s."), *GetName(), *StateClass->GetName());
			bValid = false;
		}
	}

	//通常攻撃は追加登録で有効化する。旧11ステートだけでも初期化は通す
	int32 NormalAttackCount = 0;
	for (UStateBase* State : GetRegisteredStates())
	{
		if (IsValid(State) && State->IsA<UPlayerNormalAttackState>())
		{
			++NormalAttackCount;
		}
	}
	if (NormalAttackCount > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Register only one PlayerNormalAttackState subclass."), *GetName());
		bValid = false;
	}
	for (UStateBase* State : GetRegisteredStates())
	{
		if (!IsValid(Cast<UPlayerStateBase>(State)))
		{
			bValid = false;
		}
	}
	bConfigured = bValid;
	if (bConfigured && !ChangePlayerState(InitialStateClass))
	{
		bConfigured = false;
		UE_LOG(LogTemp, Error, TEXT("%s: Initial player state refused entry."), *GetName());
	}
}

//-----------------------------------------------------
//ステート変更処理
//-----------------------------------------------------
bool UPlayerStateMachine::ChangePlayerState(TSubclassOf<UPlayerStateBase> NewStateClass)
{
	UE_LOG(LogTemp, Error, TEXT("ステート変更できません"), bConfigured);
	UE_LOG(LogTemp, Error, TEXT("ステート変更できません"), bTransitionInProgress);

	if (!bConfigured || bTransitionInProgress)
	{
		return false;
	}

	//開始や終了処理から重ねてステートを変更しないようにする
	TGuardValue<bool> TransitionGuard(bTransitionInProgress, true);
	UPlayerStateBase* State = GetPlayerState(NewStateClass);
	if (!State || IsCurrentState<UPlayerDeadState>())
	{
		return false;
	}

	//被ダメージと死亡は終了待ち中でも割り込む
	if (State->IsA<UPlayerDamageState>() || State->IsA<UPlayerDeadState>())
	{
		return ForceChangeState(State);
	}
	return ChangeState(State);
}

//-----------------------------------------------------
//移動系ステートか
//-----------------------------------------------------
bool UPlayerStateMachine::IsLocomotionState() const
{
	return bConfigured && IsCurrentState<UPlayerLocomotionState>();
}

//-----------------------------------------------------
//停止処理
//-----------------------------------------------------
void UPlayerStateMachine::Stop()
{
	if (bTransitionInProgress)
	{
		return;
	}

	//開始や終了処理から重ねてステートを変更しないようにする
	TGuardValue<bool> TransitionGuard(bTransitionInProgress, true);
	Super::Stop();
	if (UPlayerDamageState* DamageState = GetState<UPlayerDamageState>())
	{
		DamageState->ClearProtection();
	}
	CancelLocomotionRequests();
	bConfigured = false;
}

//-----------------------------------------------------
//登録済みステート取得
//-----------------------------------------------------
UPlayerStateBase* UPlayerStateMachine::GetPlayerState(TSubclassOf<UPlayerStateBase> StateClass) const
{
	if (!StateClass)
	{
		return nullptr;
	}

	//親クラス指定でも登録したBPのステートを返す
	for (UStateBase* State : GetRegisteredStates())
	{
		if (IsValid(State) && State->IsA(StateClass))
		{
			return Cast<UPlayerStateBase>(State);
		}
	}
	return nullptr;
}

//-----------------------------------------------------
//操作可能か
//-----------------------------------------------------
bool UPlayerStateMachine::CanControl() const
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	const UPlayerStateBase* State = Cast<UPlayerStateBase>(GetCurrentState());
	return bConfigured && IsValid(PlayerCharacter) && PlayerCharacter->m_bCanControl && State && State->AllowsManualControl();
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerStateMachine::Update(float DeltaTime)
{
	if (!bConfigured)
	{
		return;
	}
	DeltaTime = FMath::Max(0.f, DeltaTime);

	//待ち時間と無敵時間は他のステート中も進める
	for (UStateBase* RegisteredState : GetRegisteredStates())
	{
		if (UPlayerStateBase* State = Cast<UPlayerStateBase>(RegisteredState))
		{
			State->BackgroundUpdate(DeltaTime);
		}
	}
	Super::Update(DeltaTime);
	UPlayerStateBase* State = Cast<UPlayerStateBase>(GetCurrentState());
	if (!IsValid(State))
	{
		return;
	}
	const float Rate = FMath::Max(0.f, State->GetStaminaConsumptionRate());

	//消費しないステート中はスタミナを回復
	if (Rate > 0.f)
	{
		StaminaUsed = FMath::Clamp(StaminaUsed + DeltaTime * Rate, 0.f, GetMaximumStamina());
		if (IsStaminaDepleted())
		{
			State->OnStaminaDepleted();
		}
	}
	else
	{
		StaminaUsed = FMath::Max(0.f, StaminaUsed - DeltaTime * FMath::Max(0.f, StaminaRecoveryRate));
	}
}

//-----------------------------------------------------
//移動ステート選択
//-----------------------------------------------------
TSubclassOf<UPlayerStateBase> UPlayerStateMachine::SelectLocomotionState(bool bAfterLanding) const
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!IsValid(PlayerCharacter))
	{
		return UPlayerIdleState::StaticClass();
	}
	const UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement();
	if (!bAfterLanding && IsValid(Movement) && Movement->IsFalling())
	{
		return UPlayerJumpState::StaticClass();
	}
	if (bCrouchRequested)
	{
		return UPlayerCrouchState::StaticClass();
	}
	if (bDashRequested && !IsStaminaDepleted() && !PlayerCharacter->m_charaMoveInput.IsNearlyZero(0.1f))
	{
		return UPlayerDashState::StaticClass();
	}
	return PlayerCharacter->m_charaMoveInput.IsNearlyZero(0.1f) ? UPlayerIdleState::StaticClass() : UPlayerWalkState::StaticClass();
}

//-----------------------------------------------------
//入力に合わせて移動ステートを変更
//-----------------------------------------------------
void UPlayerStateMachine::RefreshLocomotionState()
{
	if (IsLocomotionState() && !IsCurrentState<UPlayerJumpState>())
	{
		ChangePlayerState(SelectLocomotionState());
	}
}

//-----------------------------------------------------
//移動ステートに戻す
//-----------------------------------------------------
bool UPlayerStateMachine::ReturnToLocomotion(bool bAfterLanding)
{
	return ChangePlayerState(SelectLocomotionState(bAfterLanding));
}

//-----------------------------------------------------
//移動入力
//-----------------------------------------------------
void UPlayerStateMachine::MoveInput()
{
	if (!CanControl())
	{
		return;
	}
	RefreshLocomotionState();
	if (UPlayerStateBase* State = Cast<UPlayerStateBase>(GetCurrentState()))
	{
		State->OnMoveInput();
	}
}

//-----------------------------------------------------
//ダッシュ入力の保存
//-----------------------------------------------------
void UPlayerStateMachine::SetDashRequested(bool bRequested)
{
	if (bRequested && (!CanControl() || !IsLocomotionState() || bCrouchRequested || IsStaminaDepleted()))
	{
		return;
	}
	bDashRequested = bRequested;
	RefreshLocomotionState();
}

//-----------------------------------------------------
//しゃがみ切り替え
//-----------------------------------------------------
void UPlayerStateMachine::ToggleCrouch()
{
	if (!CanControl() || !IsLocomotionState())
	{
		return;
	}
	bCrouchRequested = !bCrouchRequested;
	RefreshLocomotionState();
}

//-----------------------------------------------------
//ジャンプ開始
//-----------------------------------------------------
void UPlayerStateMachine::TryJump()
{
	if (!CanControl() || !IsLocomotionState() || IsCurrentState<UPlayerJumpState>())
	{
		return;
	}
	bJumpRequested = true;
	ChangePlayerState(UPlayerJumpState::StaticClass());
	bJumpRequested = false;
}

//-----------------------------------------------------
//攻撃開始
//-----------------------------------------------------
void UPlayerStateMachine::TryAttack()
{
	if (!CanControl())
	{
		return;
	}
	if (IsCurrentState<UPlayerShadowState>() && !ReturnToLocomotion())
	{
		return;
	}

	//ステルスキルできる相手がいる場合だけ従来の攻撃を使う
	UPlayerAttackState* SneakAttack = GetState<UPlayerAttackState>();
	if (SneakAttack && SneakAttack->CanEnter())
	{
		ChangePlayerState(UPlayerAttackState::StaticClass());
		return;
	}
	if (!GetState<UPlayerNormalAttackState>())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Register a PlayerNormalAttackState BP in State Classes to use normal attacks."), *GetName());
		return;
	}
	ChangePlayerState(UPlayerNormalAttackState::StaticClass());
}

//-----------------------------------------------------
//影状態切り替え
//-----------------------------------------------------
void UPlayerStateMachine::ToggleShadow()
{
	if (!CanControl())
	{
		return;
	}
	if (IsCurrentState<UPlayerShadowState>())
	{
		ReturnToLocomotion();
	}
	else
	{
		ChangePlayerState(UPlayerShadowState::StaticClass());
	}
}

//-----------------------------------------------------
//インタラクト開始
//-----------------------------------------------------
void UPlayerStateMachine::TryInteract()
{
	if (CanControl() && IsLocomotionState() && !IsCurrentState<UPlayerJumpState>())
	{
		ChangePlayerState(UPlayerInteractState::StaticClass());
	}
}

//-----------------------------------------------------
//着地通知
//-----------------------------------------------------
void UPlayerStateMachine::NotifyLanded()
{
	if (IsCurrentState<UPlayerJumpState>())
	{
		ReturnToLocomotion(true);
	}
}

//-----------------------------------------------------
//攻撃終了通知
//-----------------------------------------------------
void UPlayerStateMachine::NotifyAttackFinished()
{
	if (!IsCurrentState<UPlayerAttackState>())
	{
		return;
	}
	if (UPlayerAttackState* State = GetState<UPlayerAttackState>())
	{
		State->NotifyAttackFinished();
	}
}

//-----------------------------------------------------
//インタラクト終了通知
//-----------------------------------------------------
void UPlayerStateMachine::NotifyInteractionFinished()
{
	if (!IsCurrentState<UPlayerInteractAnimationState>())
	{
		return;
	}
	if (UPlayerInteractAnimationState* State = GetState<UPlayerInteractAnimationState>())
	{
		State->CompleteInteraction();
	}
}

//-----------------------------------------------------
//被ダメージ処理
//-----------------------------------------------------
void UPlayerStateMachine::ReceiveDamage(int32 Amount, const FVector& Knockback)
{
	if (UPlayerDamageState* State = GetState<UPlayerDamageState>())
	{
		State->ReceiveDamage(Amount, Knockback);
	}
}

//-----------------------------------------------------
//無敵中か
//-----------------------------------------------------
bool UPlayerStateMachine::IsInvincible() const
{
	const UPlayerDamageState* State = GetState<UPlayerDamageState>();
	return State && State->IsInvincible();
}

//-----------------------------------------------------
//攻撃中か
//-----------------------------------------------------
bool UPlayerStateMachine::IsAttacking() const
{
	const UPlayerAttackState* State = GetState<UPlayerAttackState>();
	if (IsCurrentState<UPlayerAttackState>())
	{
		return State && State->IsAttackActive();
	}
	const UPlayerNormalAttackState* NormalAttack = GetState<UPlayerNormalAttackState>();
	return IsCurrentState<UPlayerNormalAttackState>() && NormalAttack && NormalAttack->IsAttackActive();
}
