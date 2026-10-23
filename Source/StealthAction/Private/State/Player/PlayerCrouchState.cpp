#include "State/Player/PlayerCrouchState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerCrouchState::UPlayerCrouchState()
{
	bOverrideCamera = true;
	CameraSettings = { FRotator(0.f, 0.f, 0.f), 60.f, 250.f, FVector(0.f, 70.f, 20.f), 5.f };
	MoveSpeed = 100.f;
	FootstepNoiseLevel = 0;
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerCrouchState::OnEnter_Implementation()
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->CancelDash(); }
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent())
		{
			Capsule->SetCapsuleHalfHeight(PlayerCharacter->m_capsuleHeight * FMath::Clamp(CapsuleHeightRatio, 0.01f, 1.f));
		}
		PlayerCharacter->m_cameraStatus = ECameraStatus::Crouch;
		PlayerCharacter->m_bCameraSwitching = true;
	}
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerCrouchState::OnExit_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (PlayerCharacter->IsUpPosWall(PlayerCharacter->GetFeetLocation()))
		{
			PlayerCharacter->SetActorLocation(PlayerCharacter->m_saveLastNotUpWallPos);
		}
		if (UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent())
		{
			Capsule->SetCapsuleHalfHeight(PlayerCharacter->m_capsuleHeight);
		}
		PlayerCharacter->m_cameraStatus = ECameraStatus::ThirdPerson;
		PlayerCharacter->m_bCameraSwitching = true;
	}
}
