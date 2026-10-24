//----------------------------------------------------------
//概要：プレイヤーステートの共通処理
//----------------------------------------------------------

#include "State/Player/PlayerStateBase.h"
#include "State/Player/PlayerStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Noise/NoiseManager.h"
#include "Engine/World.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

//-----------------------------------------------------
//ワールド取得
//-----------------------------------------------------
UWorld* UPlayerStateBase::GetWorld() const
{
	return IsValid(Player) ? Player->GetWorld() : nullptr;
}

//-----------------------------------------------------
//初期化
//-----------------------------------------------------
void UPlayerStateBase::Initialize(UStateMachine* StateMachine, AActor* Owner)
{
	Super::Initialize(StateMachine, Owner);
	Player = Cast<APlayerCharacter>(Owner);
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerStateBase::Enter()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
		{
			//このステートの移動速度を反映
			Movement->MaxWalkSpeed = FMath::Max(0.f, MoveSpeed);
		}
	}
	if (bOverrideCamera)
	{
		if (APlayerCharacter* PlayerCharacter = GetPlayer())
		{
			PlayerCharacter->m_bCameraSwitching = true;
		}
	}
	Super::Enter();
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerStateBase::Exit()
{
	Super::Exit();
	if (bOverrideCamera)
	{
		if (APlayerCharacter* PlayerCharacter = GetPlayer())
		{
			PlayerCharacter->m_bCameraSwitching = true;
		}
	}
}

//-----------------------------------------------------
//開始可能か
//-----------------------------------------------------
bool UPlayerStateBase::CanEnter() const
{
	return IsValid(Player) && Super::CanEnter();
}

//-----------------------------------------------------
//遷移可能か
//-----------------------------------------------------
bool UPlayerStateBase::CanTransitionTo(const UStateBase* NewState) const
{
	const UPlayerStateBase* PlayerState = Cast<UPlayerStateBase>(NewState);
	return PlayerState && Super::CanTransitionTo(NewState);
}

//-----------------------------------------------------
//ステート変更要求
//-----------------------------------------------------
bool UPlayerStateBase::RequestPlayerState(TSubclassOf<UPlayerStateBase> NewStateClass)
{
	UPlayerStateMachine* Machine = Cast<UPlayerStateMachine>(m_pStateMachine);
	return Machine && Machine->ChangePlayerState(NewStateClass);
}

//-----------------------------------------------------
//プレイヤー取得
//-----------------------------------------------------
APlayerCharacter* UPlayerStateBase::GetPlayer() const
{
	return IsValid(Player) ? Player.Get() : nullptr;
}

//-----------------------------------------------------
//ステートマシン取得
//-----------------------------------------------------
UPlayerStateMachine* UPlayerStateBase::GetPlayerMachine() const
{
	return Cast<UPlayerStateMachine>(m_pStateMachine);
}

//-----------------------------------------------------
//移動入力処理
//-----------------------------------------------------
void UPlayerStateBase::OnMoveInput_Implementation()
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter || !bAllowMovementInput || !PlayerCharacter->CanReceivePlayerInput())
	{
		return;
	}
	UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement();
	UWorld* World = GetWorld();
	if (!IsValid(Movement) || !World || !Movement->IsMovingOnGround() || PlayerCharacter->m_charaMoveInput.IsNearlyZero(0.1f))
	{
		return;
	}

	//カメラの向きに合わせて移動方向を決める
	const float Yaw = PlayerCharacter->m_cameraStatus == ECameraStatus::TopDownView ? PlayerCharacter->GetActorRotation().Yaw
																					: PlayerCharacter->GetControlRotation().Yaw;
	const FRotator Rotation(0.f, Yaw, 0.f);
	FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X) * PlayerCharacter->m_charaMoveInput.Y +
						FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y) * PlayerCharacter->m_charaMoveInput.X;
	Direction.Z = 0.f;
	if (!Direction.Normalize())
	{
		return;
	}
	PlayerCharacter->AddMovementInput(Direction, 1.f);
	PlayerCharacter->SetActorRotation(FMath::RInterpTo(PlayerCharacter->GetActorRotation(), Direction.Rotation(), World->GetDeltaSeconds(),
													   FMath::Max(0.f, RotationInterpSpeed)));
	if (FootstepNoiseLevel > 0)
	{
		if (UNoiseManager* Noise = World->GetSubsystem<UNoiseManager>())
		{
			Noise->MakeNoise(FootstepNoiseLevel, PlayerCharacter->GetActorLocation());
		}
	}
}

//-----------------------------------------------------
//スタミナ切れ処理
//-----------------------------------------------------
void UPlayerStateBase::OnStaminaDepleted_Implementation()
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine())
	{
		Machine->CancelDash();
		Machine->ReturnToLocomotion();
	}
}
