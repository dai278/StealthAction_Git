#include "State/Player/PlayerInteractAnimationState.h"
#include "State/Player/PlayerInteractState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Interact/Interact.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

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
	bMontageStarted = false;
	bMontageEnded = false;
	bMontageCancelled = false;
	PlayingAnimInstance.Reset();
	if (!InteractionTarget.IsValid())
	{
		if (APlayerCharacter* PlayerCharacter = GetPlayer()) { InteractionTarget = PlayerCharacter->m_hitInteractOb; }
	}
	if (IsValid(InteractionMontage))
	{
		APlayerCharacter* PlayerCharacter = GetPlayer();
		USkeletalMeshComponent* Mesh = PlayerCharacter ? PlayerCharacter->GetMesh() : nullptr;
		UAnimInstance* AnimInstance = IsValid(Mesh) ? Mesh->GetAnimInstance() : nullptr;
		if (IsValid(AnimInstance) && AnimInstance->Montage_Play(InteractionMontage.Get(), FMath::Max(0.01f, MontagePlayRate)) > 0.f)
		{
			PlayingAnimInstance = AnimInstance;
			bMontageStarted = true;
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UPlayerInteractAnimationState::OnInteractionMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, InteractionMontage.Get());
		}
		else
		{
			//再生できなかった場合は対象を実行せずに戻す
			bMontageCancelled = true;
			UE_LOG(LogTemp, Warning, TEXT("%s: Interaction montage could not be played."), *GetName());
		}
	}
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerInteractAnimationState::OnUpdate_Implementation(float DeltaTime)
{
	if (!InteractionTarget.IsValid() || bMontageCancelled || (bMontageStarted && !PlayingAnimInstance.IsValid()))
	{
		if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->ReturnToLocomotion(); }
		return;
	}
	if (bInteractionExecuted) { CompleteInteraction(); return; }
	if (bMontageStarted)
	{
		if (!bMontageEnded) { return; }
		if (bWaitForAnimationNotify)
		{
			//通知が無いまま再生が終わった場合は実行せずに解除
			UE_LOG(LogTemp, Warning, TEXT("%s: Interaction montage ended without the interaction notify."), *GetName());
			if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->ReturnToLocomotion(); }
		}
		else { CompleteInteraction(); }
		return;
	}
	//モンタージュ未設定の場合は従来の時間待ちを使う
	if (!bWaitForAnimationNotify && m_stateTimer >= FMath::Max(0.f, InteractionDuration)) { CompleteInteraction(); }
}

//-----------------------------------------------------
//インタラクト実行
//-----------------------------------------------------
void UPlayerInteractAnimationState::CompleteInteraction()
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UPlayerStateMachine* Machine = GetPlayerMachine();
	if (!PlayerCharacter || !Machine || bMontageCancelled || !Machine->IsCurrentState<UPlayerInteractAnimationState>()) { return; }
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
void UPlayerInteractAnimationState::OnExit_Implementation()
{
	if (UAnimInstance* AnimInstance = PlayingAnimInstance.Get())
	{
		//停止通知でインタラクトを実行しないよう先に解除する
		FOnMontageEnded EmptyDelegate;
		AnimInstance->Montage_SetEndDelegate(EmptyDelegate, InteractionMontage.Get());
		if (AnimInstance->Montage_IsActive(InteractionMontage.Get()))
		{
			AnimInstance->Montage_Stop(FMath::Max(0.f, MontageBlendOutTime), InteractionMontage.Get());
		}
	}
	PlayingAnimInstance.Reset();
	bMontageStarted = false;
	bMontageEnded = false;
	InteractionTarget.Reset();
}

//-----------------------------------------------------
//モンタージュ終了通知
//-----------------------------------------------------
void UPlayerInteractAnimationState::OnInteractionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bMontageStarted || Montage != InteractionMontage.Get()) { return; }
	//開始や終了処理の途中ではステートを変更しない
	bMontageEnded = true;
	bMontageCancelled = bInterrupted;
}
