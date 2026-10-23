#include "State/Player/PlayerShadowState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Light/ExtendedSpotLightManager.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

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
	//移動用のカプセルは触らずに見た目だけ変更する
	bOriginalMeshVisible = Mesh->IsVisible();
	OriginalMeshLocation = Mesh->GetRelativeLocation();
	SinkElapsed = 0.f;
	OriginalMaterials.Reset();
	SinkMaterials.Reset();
	OriginalOpacities.Reset();
	bool bMissingOpacity = false;
	for (int32 Index = 0; Index < Mesh->GetNumMaterials(); ++Index)
	{
		UMaterialInterface* Material = Mesh->GetMaterial(Index);
		OriginalMaterials.Add(Material);
		float Opacity = 1.f;
		UMaterialInstanceDynamic* DynamicMaterial = nullptr;
		if (IsValid(Material) && Material->GetScalarParameterValue(FMaterialParameterInfo(OpacityParameterName), Opacity))
		{
			//元のマテリアルの値は変更しない
			DynamicMaterial = Mesh->CreateDynamicMaterialInstance(Index);
		}
		else if (IsValid(Material)) { bMissingOpacity = true; }
		SinkMaterials.Add(DynamicMaterial);
		OriginalOpacities.Add(Opacity);
	}
	if (bMissingOpacity)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Mesh materials need a scalar parameter '%s' connected to opacity for shadow fading."), *GetName(), *OpacityParameterName.ToString());
	}
	OriginalMeshCollision = Mesh->GetCollisionEnabled();
	bOriginalOverlapEvents = Mesh->GetGenerateOverlapEvents();
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
	PlayerCharacter->m_cameraStatus = ECameraStatus::InShadow;
	PlayerCharacter->m_bCameraSwitching = true;
	PlayerCharacter->m_saveLastNotUpWallPos = PlayerCharacter->GetActorLocation();
	//沈むメッシュではなく移動用のルートへ追従させる
	StopShadowEffect();
	if (IsValid(ShadowEffectSystem))
	{
		ShadowEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
			ShadowEffectSystem.Get(), PlayerCharacter->GetRootComponent(), NAME_None,
			PlayerCharacter->GetFeetLocation() + ShadowEffectOffset, FRotator::ZeroRotator,
			EAttachLocation::KeepWorldPosition, true, false, ENCPoolMethod::None, false);
		if (IsValid(ShadowEffect))
		{
			ShadowEffect->SetVariableFloat(ShadowAlphaParameterName, SinkDuration > 0.f ? 0.f : 1.f);
			ShadowEffect->Activate(true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Set Shadow Effect System in the shadow state BP."), *GetName());
	}
	UpdateSinkAppearance(0.f);
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerShadowState::OnUpdate_Implementation(float DeltaTime)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UWorld* World = GetWorld();
	if (!PlayerCharacter) { return; }
	UpdateSinkAppearance(DeltaTime);
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
	StopShadowEffect();
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
		//途中で解除されても位置とマテリアルを元に戻す
		Mesh->SetRelativeLocation(OriginalMeshLocation);
		for (int32 Index = 0; Index < OriginalMaterials.Num(); ++Index)
		{
			Mesh->SetMaterial(Index, OriginalMaterials[Index].Get());
		}
		Mesh->SetVisibility(bOriginalMeshVisible);
		Mesh->SetCollisionEnabled(OriginalMeshCollision);
		Mesh->SetGenerateOverlapEvents(bOriginalOverlapEvents);
	}
	OriginalMaterials.Reset();
	SinkMaterials.Reset();
	OriginalOpacities.Reset();

	PlayerCharacter->m_cameraStatus = ECameraStatus::ThirdPerson;
	PlayerCharacter->m_bCameraSwitching = true;
}

//-----------------------------------------------------
//影に潜る演出
//-----------------------------------------------------
void UPlayerShadowState::UpdateSinkAppearance(float DeltaTime)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	USkeletalMeshComponent* Mesh = PlayerCharacter ? PlayerCharacter->GetMesh() : nullptr;
	if (!bShadowApplied || !IsValid(Mesh)) { return; }
	SinkElapsed += FMath::Max(0.f, DeltaTime);
	const float Alpha = SinkDuration > 0.f ? FMath::Clamp(SinkElapsed / SinkDuration, 0.f, 1.f) : 1.f;
	//プレイヤーの移動に付いていきながら下へ沈む
	Mesh->SetRelativeLocation(OriginalMeshLocation - FVector(0.f, 0.f, FMath::Max(0.f, SinkDistance) * Alpha));
	for (int32 Index = 0; Index < SinkMaterials.Num(); ++Index)
	{
		if (IsValid(SinkMaterials[Index]))
		{
			SinkMaterials[Index]->SetScalarParameterValue(OpacityParameterName, OriginalOpacities[Index] * (1.f - Alpha));
		}
	}
	if (IsValid(ShadowEffect))
	{
		ShadowEffect->SetWorldLocation(PlayerCharacter->GetFeetLocation() + ShadowEffectOffset);
		ShadowEffect->SetVariableFloat(ShadowAlphaParameterName, Alpha);
	}
	//潜り終わったら完全に非表示にする
	Mesh->SetVisibility(bOriginalMeshVisible && Alpha < 1.f);
}

//-----------------------------------------------------
//影エフェクト停止
//-----------------------------------------------------
void UPlayerShadowState::StopShadowEffect()
{
	if (IsValid(ShadowEffect))
	{
		//残った粒子はその場で消し、完了後に自動破棄する
		ShadowEffect->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		ShadowEffect->SetAutoDestroy(true);
		ShadowEffect->Deactivate();
	}
	ShadowEffect = nullptr;
}
