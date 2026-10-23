#include "State/Player/PlayerInteractAnimationState.h"
#include "State/Player/PlayerInteractState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Interact/Interact.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerInteractAnimationState::UPlayerInteractAnimationState()
{
	PlayerStatus = EPlayerStatus::InteractAnimation;
	MoveSpeed = 0.f;
	bAllowManualControl = false;
}

//-----------------------------------------------------
//対象を保存してインタラクト開始
//-----------------------------------------------------
bool UPlayerInteractAnimationState::BeginInteraction(AInteract* Target)
{
	InteractionTarget = Target;
	const bool bStarted = RequestPlayerState(UPlayerInteractAnimationState::StaticClass());
	if (!bStarted) { InteractionTarget.Reset(); }
	return bStarted;
}

//-----------------------------------------------------
//開始条件
//-----------------------------------------------------
bool UPlayerInteractAnimationState::CanEnterState_Implementation() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	return InteractionTarget.IsValid() || (PlayerCharacter && IsValid(PlayerCharacter->m_hitInteractOb));
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerInteractAnimationState::OnEnter_Implementation()
{
	bInteractionExecuted = false;
	if (!InteractionTarget.IsValid())
	{
		if (APlayerCharacter* PlayerCharacter = GetPlayer()) { InteractionTarget = PlayerCharacter->m_hitInteractOb; }
	}
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerInteractAnimationState::OnUpdate_Implementation(float DeltaTime)
{
	if (!InteractionTarget.IsValid())
	{
		if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->ReturnToLocomotion(); }
		return;
	}
	if (bInteractionExecuted || (!bWaitForAnimationNotify && m_stateTimer >= FMath::Max(0.f, InteractionDuration))) { CompleteInteraction(); }
}

//-----------------------------------------------------
//インタラクト実行
//-----------------------------------------------------
void UPlayerInteractAnimationState::CompleteInteraction()
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UPlayerStateMachine* Machine = GetPlayerMachine();
	if (!PlayerCharacter || !Machine || !Machine->IsCurrentState<UPlayerInteractAnimationState>()) { return; }
	//終了を待つ間に何度も実行しないようにする
	if (!bInteractionExecuted)
	{
		bInteractionExecuted = true;
		if (InteractionTarget.IsValid()) { InteractionTarget->Interact(PlayerCharacter); }
	}
	if (IsValid(Machine) && Machine->IsCurrentState<UPlayerInteractAnimationState>()) { Machine->ReturnToLocomotion(); }
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerInteractAnimationState::OnExit_Implementation() { InteractionTarget.Reset(); }
