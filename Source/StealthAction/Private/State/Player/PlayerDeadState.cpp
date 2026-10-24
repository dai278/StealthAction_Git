#include "State/Player/PlayerDeadState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "UI/Transition/LevelTransitionSubsystem.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerDeadState::UPlayerDeadState()
{
	PlayerStatus = EPlayerStatus::Dead;
	MoveSpeed = 0.f;
	bAllowManualControl = false;
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerDeadState::OnEnter_Implementation()
{
	bTransitionRequested = false;
	FadeStartTime = FMath::Max(0.f, GameOverDelay);
	PlayingAnimInstance.Reset();
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->CancelLocomotionRequests(); }
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter) { return; }
	PlayerCharacter->m_playerInfo.isAlive = false;
	PlayerCharacter->m_playerInfo.bIsGetKeyItem = 0;
	if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement()) { Movement->StopMovementImmediately(); }
	USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh();
	UAnimInstance* AnimInstance = IsValid(Mesh) ? Mesh->GetAnimInstance() : nullptr;
	if (IsValid(DeathMontage))
	{
		const float Duration = IsValid(AnimInstance)
			? AnimInstance->Montage_Play(DeathMontage.Get(), FMath::Max(0.01f, MontagePlayRate), EMontagePlayReturnType::Duration) : 0.f;
		if (Duration > 0.f)
		{
			PlayingAnimInstance = AnimInstance;
			if (bWaitForDeathMontage) { FadeStartTime += Duration; }
		}
		else
		{
			//再生失敗で死亡画面へ進めなくならないようにする
			UE_LOG(LogTemp, Warning, TEXT("%s: Death montage could not be played. Continuing to game over."), *GetName());
		}
	}
	if (FadeStartTime <= 0.f) { BeginGameOverTransition(); }
}

//-----------------------------------------------------
//死亡演出を見せてから暗転する
//-----------------------------------------------------
void UPlayerDeadState::OnUpdate_Implementation(float DeltaTime)
{
	if (!bTransitionRequested && m_stateTimer >= FadeStartTime) { BeginGameOverTransition(); }
}

//-----------------------------------------------------
//ゲームオーバーへの移動開始
//-----------------------------------------------------
void UPlayerDeadState::BeginGameOverTransition()
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (bTransitionRequested || !bOpenGameOverLevel || GameOverLevel.IsNone() || !PlayerCharacter || !GetWorld()) { return; }
	//OpenLevelで終了処理が呼ばれても重ねて要求しない
	bTransitionRequested = true;
	UGameInstance* Instance = PlayerCharacter->GetGameInstance();
	ULevelTransitionSubsystem* Transition = Instance ? Instance->GetSubsystem<ULevelTransitionSubsystem>() : nullptr;
	if (!Transition || !Transition->OpenLevelWithFade(GameOverLevel, FadeOutDuration, BlackHoldDuration, FadeInDuration, FadeColor))
	{
		UGameplayStatics::OpenLevel(PlayerCharacter, GameOverLevel);
	}
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerDeadState::OnExit_Implementation()
{
	bTransitionRequested = true;
	if (UAnimInstance* AnimInstance = PlayingAnimInstance.Get())
	{
		if (AnimInstance->Montage_IsActive(DeathMontage.Get())) { AnimInstance->Montage_Stop(0.f, DeathMontage.Get()); }
	}
	PlayingAnimInstance.Reset();
}
