#include "State/Player/PlayerShadowState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Light/ExtendedSpotLightManager.h"
#include "Engine/World.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerShadowState::UPlayerShadowState()
{
	PlayerStatus = EPlayerStatus::InShadow;
	bOverrideCamera = true;
	CameraSettings = { FRotator(0.f, 0.f, 0.f), 60.f, 300.f, FVector(0.f, 70.f, 20.f), 5.f };
	bAllowMovementInput = true;
	StaminaConsumptionRate = 1.f;
}

//-----------------------------------------------------
//開始条件
//-----------------------------------------------------
bool UPlayerShadowState::CanEnterState_Implementation() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	const UPlayerStateMachine* Machine = GetPlayerMachine();
	UWorld* World = GetWorld();
	if (!PlayerCharacter || !Machine || !World || Machine->IsStaminaDepleted()
		|| !IsValid(PlayerCharacter->GetCapsuleComponent()) || !IsValid(PlayerCharacter->GetMesh())) { return false; }
	UExtendedSpotLightManager* Lights = World->GetSubsystem<UExtendedSpotLightManager>();
	const FVector Feet = PlayerCharacter->GetFeetLocation();
	return IsValid(Lights) && (!Lights->IsHitAllLight(Feet)
		|| (PlayerCharacter->m_bOnShadow && !Lights->IsHitEnemyLight(Feet)));
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerShadowState::OnEnter_Implementation()
{
	bShadowApplied = false;
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter) { return; }
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->CancelLocomotionRequests(); }
	UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();
	USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh();
	if (!IsValid(Capsule) || !IsValid(Mesh)) { return; }
	bShadowApplied = true;
	//サイズは変えずに影用のコリジョンへ切り替える
	OriginalCollisionProfile = Capsule->GetCollisionProfileName();
	Capsule->SetCollisionProfileName(ShadowCollisionProfile);
	OriginalMesh = Mesh->GetSkeletalMeshAsset();
	OriginalMeshCollision = Mesh->GetCollisionEnabled();
	bOriginalOverlapEvents = Mesh->GetGenerateOverlapEvents();
	if (ShadowMesh) { Mesh->SetSkeletalMesh(ShadowMesh.Get()); }
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
	PlayerCharacter->m_cameraStatus = ECameraStatus::InShadow;
	PlayerCharacter->m_bCameraSwitching = true;
	PlayerCharacter->m_saveLastNotUpWallPos = PlayerCharacter->GetActorLocation();
	if (IsValid(PlayerCharacter->m_ShadowEffectChild))
	{
		PlayerCharacter->m_ShadowEffectChild->SetChildActorClass(ShadowEffectClass);
		PlayerCharacter->m_ShadowEffectChild->SetWorldLocation(PlayerCharacter->GetFeetLocation());
		if (AActor* Effect = PlayerCharacter->m_ShadowEffectChild->GetChildActor()) { Effect->SetActorHiddenInGame(false); }
	}
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerShadowState::OnUpdate_Implementation(float DeltaTime)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UWorld* World = GetWorld();
	if (!PlayerCharacter) { return; }
	UExtendedSpotLightManager* Lights = World ? World->GetSubsystem<UExtendedSpotLightManager>() : nullptr;
	if (!IsValid(Lights) || Lights->IsHitAllLight(PlayerCharacter->GetFeetLocation())
		|| (PlayerCharacter->m_bOnShadow && Lights->IsHitEnemyLight(PlayerCharacter->GetFeetLocation())))
	{
		if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->ReturnToLocomotion(); }
	}
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerShadowState::OnExit_Implementation()
{
	if (!bShadowApplied) { return; }
	bShadowApplied = false;
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter) { return; }
	//障害物の中で解除した場合は最後に戻れた位置へ戻す
	if (PlayerCharacter->IsUpPosWall(PlayerCharacter->GetFeetLocation())) { PlayerCharacter->SetActorLocation(PlayerCharacter->m_saveLastNotUpWallPos); }
	if (UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent())
	{
		Capsule->SetCollisionProfileName(OriginalCollisionProfile);
	}
	if (USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh())
	{
		Mesh->SetSkeletalMesh(OriginalMesh.Get());
		Mesh->SetCollisionEnabled(OriginalMeshCollision);
		Mesh->SetGenerateOverlapEvents(bOriginalOverlapEvents);
	}
	OriginalMesh = nullptr;
	if (IsValid(PlayerCharacter->m_ShadowEffectChild))
	{
		if (AActor* Effect = PlayerCharacter->m_ShadowEffectChild->GetChildActor()) { Effect->SetActorHiddenInGame(true); }
	}
	PlayerCharacter->m_cameraStatus = ECameraStatus::ThirdPerson;
	PlayerCharacter->m_bCameraSwitching = true;
}
