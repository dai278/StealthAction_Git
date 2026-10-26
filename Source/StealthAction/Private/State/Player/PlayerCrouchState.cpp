//担当日髙
//----------------------------------------------------------
//概要：しゃがみステート
//----------------------------------------------------------

#include "State/Player/PlayerCrouchState.h"
#include "State/Player/PlayerStateMachine.h"
#include "State/Player/PlayerShadowState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerCrouchState::UPlayerCrouchState()
{
	PlayerStatus = EPlayerStatus::Crouch;
	bOverrideCamera = true;
	CameraSettings = {FRotator(0.f, 0.f, 0.f), 60.f, 250.f, FVector(0.f, 70.f, 20.f), 5.f};
	MoveSpeed = 100.f;
	FootstepNoiseLevel = 0;
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerCrouchState::OnEnter_Implementation()
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine())
	{
		Machine->CancelDash();
	}
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		ResizeCapsuleAtFeet(PlayerCharacter->m_capsuleHeight * FMath::Clamp(CapsuleHeightRatio, 0.01f, 1.f),
			PlayerCharacter->GetFeetLocation());
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
		UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();
		if (!IsValid(Capsule))
		{
			return;
		}
		FVector Feet = PlayerCharacter->GetFeetLocation();
		const UPlayerStateMachine* Machine = GetPlayerMachine();
		const UPlayerStateBase* NextState = Machine ? Machine->GetTransitionTarget() : nullptr;
		const bool bEnteringShadow = NextState && NextState->IsA<UPlayerShadowState>();

		//影へは同じ足元から潜る。通常状態へ戻す場合だけ退避位置を使う
		if (!bEnteringShadow && PlayerCharacter->IsUpPosWall(Feet))
		{
			const float Scale = Capsule->GetScaledCapsuleHalfHeight() / FMath::Max(1.f, Capsule->GetUnscaledCapsuleHalfHeight());
			const float StandingHeight = FMath::Max(Capsule->GetUnscaledCapsuleRadius(), PlayerCharacter->m_capsuleHeight) * Scale;
			Feet = PlayerCharacter->m_saveLastNotUpWallPos - FVector(0.f, 0.f, StandingHeight);
		}
		ResizeCapsuleAtFeet(PlayerCharacter->m_capsuleHeight, Feet);
		PlayerCharacter->m_cameraStatus = ECameraStatus::ThirdPerson;
		PlayerCharacter->m_bCameraSwitching = true;
	}
}

//-----------------------------------------------------
//足元を固定してカプセルの高さを変更
//-----------------------------------------------------
void UPlayerCrouchState::ResizeCapsuleAtFeet(float HalfHeight, const FVector& Feet)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UCapsuleComponent* Capsule = PlayerCharacter ? PlayerCharacter->GetCapsuleComponent() : nullptr;
	if (!IsValid(Capsule))
	{
		return;
	}

	const float PreviousHeight = Capsule->GetScaledCapsuleHalfHeight();
	Capsule->SetCapsuleHalfHeight(HalfHeight);
	const float NewHeight = Capsule->GetScaledCapsuleHalfHeight();
	PlayerCharacter->SetActorLocation(Feet + FVector(0.f, 0.f, NewHeight));
	if (USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh())
	{
		//カプセルの中心が動いた分だけメッシュを戻す
		Mesh->AddWorldOffset(FVector(0.f, 0.f, PreviousHeight - NewHeight));
	}
}
