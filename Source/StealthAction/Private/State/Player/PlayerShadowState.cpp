//----------------------------------------------------------
//概要：影ステート
//----------------------------------------------------------

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
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerShadowState::UPlayerShadowState()
{
	PlayerStatus = EPlayerStatus::InShadow;
	bOverrideCamera = true;
	CameraSettings = {FRotator(0.f, 0.f, 0.f), 60.f, 300.f, FVector(0.f, 70.f, 20.f), 5.f};
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
	if (!PlayerCharacter || !Machine || !World || Machine->IsStaminaDepleted() || !IsValid(PlayerCharacter->GetCapsuleComponent()) ||
		!IsValid(PlayerCharacter->GetMesh()))
	{
		return false;
	}

	UExtendedSpotLightManager* Lights = World->GetSubsystem<UExtendedSpotLightManager>();

	const FVector Feet = PlayerCharacter->GetFeetLocation();

	return IsValid(Lights) && (!Lights->IsHitAllLight(Feet) || (PlayerCharacter->m_bOnShadow && !Lights->IsHitEnemyLight(Feet)));
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerShadowState::OnEnter_Implementation()
{
	bShadowApplied = false;
	APlayerCharacter* PlayerCharacter = GetPlayer();

	if (!PlayerCharacter)
	{
		return;
	}
	if (UPlayerStateMachine* Machine = GetPlayerMachine())
	{
		Machine->CancelLocomotionRequests();
	}

	UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();
	USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh();

	if (!IsValid(Capsule) || !IsValid(Mesh))
	{
		return;
	}

	bShadowApplied = true;

	//Animationの停止
	Mesh->bPauseAnims = true;

	//足元と元のサイズを保存
	LastSafeFeet = PlayerCharacter->GetFeetLocation();
	OriginalCapsuleRadius = Capsule->GetUnscaledCapsuleRadius();
	OriginalCapsuleHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	OriginalMeshLocation = Mesh->GetRelativeLocation();
	const float PreviousHeight = Capsule->GetScaledCapsuleHalfHeight();

	//高さだけでなく半径も小さくする
	OriginalCollisionProfile = Capsule->GetCollisionProfileName();
	Capsule->SetCollisionProfileName(ShadowCollisionProfile);
	const float Radius = FMath::Max(1.f, ShadowCapsuleRadius);
	Capsule->SetCapsuleSize(Radius, FMath::Max(Radius, ShadowCapsuleHalfHeight));
	const float HeightDifference = PreviousHeight - Capsule->GetScaledCapsuleHalfHeight();
	PlayerCharacter->SetActorLocation(PlayerCharacter->GetActorLocation() - FVector(0.f, 0.f, HeightDifference));

	//カプセルが下がった分だけメッシュを補正してから沈める
	Mesh->AddWorldOffset(FVector(0.f, 0.f, HeightDifference));
	SinkStartMeshLocation = Mesh->GetRelativeLocation();
	bOriginalMeshVisible = Mesh->IsVisible();
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
			//元が動的マテリアルでも演出用は別に作る
			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, Mesh);
			if (IsValid(DynamicMaterial))
			{
				Mesh->SetMaterial(Index, DynamicMaterial);
			}
		}
		else if (IsValid(Material))
		{
			bMissingOpacity = true;
		}
		SinkMaterials.Add(DynamicMaterial);
		OriginalOpacities.Add(Opacity);
	}
	if (bMissingOpacity)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Mesh materials need a scalar parameter '%s' connected to opacity for shadow fading."),
			   *GetName(), *OpacityParameterName.ToString());
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
		ShadowEffect =
			UNiagaraFunctionLibrary::SpawnSystemAttached(ShadowEffectSystem.Get(), PlayerCharacter->GetRootComponent(), NAME_None,
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
	SpawnTransitionEffect(EnterShadowEffectSystem.Get(), EnterShadowEffectOffset);
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerShadowState::OnUpdate_Implementation(float DeltaTime)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UWorld* World = GetWorld();
	if (!PlayerCharacter)
	{
		return;
	}
	UpdateSinkAppearance(DeltaTime);
	const FVector Feet = PlayerCharacter->GetFeetLocation();
	if (CanRestoreAtFeet(Feet))
	{
		LastSafeFeet = Feet;
	}
	UExtendedSpotLightManager* Lights = World ? World->GetSubsystem<UExtendedSpotLightManager>() : nullptr;
	if (!IsValid(Lights) || Lights->IsHitAllLight(PlayerCharacter->GetFeetLocation()) ||
		(PlayerCharacter->m_bOnShadow && Lights->IsHitEnemyLight(PlayerCharacter->GetFeetLocation())))
	{
		if (UPlayerStateMachine* Machine = GetPlayerMachine())
		{
			Machine->ReturnToLocomotion();
		}
	}
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerShadowState::OnExit_Implementation()
{
	if (!bShadowApplied)
	{
		return;
	}
	bShadowApplied = false;
	StopShadowEffect();
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter)
	{
		return;
	}

	//狭い場所で解除した場合は最後に戻れる位置へ戻す
	const FVector Feet = PlayerCharacter->GetFeetLocation();
	const FVector RestoreFeet = CanRestoreAtFeet(Feet) ? Feet : LastSafeFeet;
	if (UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent())
	{
		Capsule->SetCapsuleSize(OriginalCapsuleRadius, OriginalCapsuleHalfHeight);
		PlayerCharacter->SetActorLocation(RestoreFeet + FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight()));
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

		//Animationの停止
		Mesh->bPauseAnims = false;
	}

	//サイズとプリセットを戻した位置で対象を取り直す
	PlayerCharacter->RefreshInteractTarget();
	OriginalMaterials.Reset();
	SinkMaterials.Reset();
	OriginalOpacities.Reset();

	PlayerCharacter->m_cameraStatus = ECameraStatus::ThirdPerson;
	PlayerCharacter->m_bCameraSwitching = true;

	//狭い場所から戻された場合も、実際に出た足元に出す
	SpawnTransitionEffect(ExitShadowEffectSystem.Get(), ExitShadowEffectOffset);
}

//-----------------------------------------------------
//出入りのエフェクトを足元に出す
//-----------------------------------------------------
void UPlayerShadowState::SpawnTransitionEffect(UNiagaraSystem* EffectSystem, const FVector& Offset)
{
	//再生が終わって消えたものを外す
	TransitionEffects.RemoveAll([](const TObjectPtr<UNiagaraComponent>& Effect)
	{
		return !IsValid(Effect);
	});

	APlayerCharacter* PlayerCharacter = GetPlayer();
	UWorld* World = GetWorld();
	if (!IsValid(EffectSystem) || !IsValid(PlayerCharacter) || !IsValid(World))
	{
		return;
	}

	//三個目を出す前に古いものから消す
	while (TransitionEffects.Num() >= 2)
	{
		if (IsValid(TransitionEffects[0]))
		{
			TransitionEffects[0]->DestroyComponent();
		}
		TransitionEffects.RemoveAt(0);
	}

	//移動しても発生した場所に残し、再生が終わったら自動で消す
	UNiagaraComponent* Effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, EffectSystem, PlayerCharacter->GetFeetLocation() + Offset,
		FRotator::ZeroRotator, FVector::OneVector, true, true, ENCPoolMethod::None, false);
	if (IsValid(Effect))
	{
		TransitionEffects.Add(Effect);
	}
}

//-----------------------------------------------------
//影に潜る演出
//-----------------------------------------------------
void UPlayerShadowState::UpdateSinkAppearance(float DeltaTime)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	USkeletalMeshComponent* Mesh = PlayerCharacter ? PlayerCharacter->GetMesh() : nullptr;
	if (!bShadowApplied || !IsValid(Mesh))
	{
		return;
	}
	SinkElapsed += FMath::Max(0.f, DeltaTime);
	const float Alpha = SinkDuration > 0.f ? FMath::Clamp(SinkElapsed / SinkDuration, 0.f, 1.f) : 1.f;

	//プレイヤーの移動に付いていきながら下へ沈む
	Mesh->SetRelativeLocation(SinkStartMeshLocation - FVector(0.f, 0.f, FMath::Max(0.f, SinkDistance) * Alpha));
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
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(ShadowEffectTailTimer);
	}

	//入り直した時に前回の余韻を残さない
	if (IsValid(StoppedShadowEffect))
	{
		StoppedShadowEffect->DestroyComponent();
	}
	StoppedShadowEffect = nullptr;

	if (IsValid(ShadowEffect))
	{
		//まず放出を止めて、残った粒子だけその場に残す
		ShadowEffect->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		ShadowEffect->SetAutoDestroy(true);
		ShadowEffect->Deactivate();

		if (IsValid(ShadowEffect))
		{
			if (World && ShadowEffectTailDuration > 0.f)
			{
				StoppedShadowEffect = ShadowEffect;
				const TWeakObjectPtr<UNiagaraComponent> Effect = StoppedShadowEffect.Get();

				//ステートが更新されていなくても時間が来たら消す
				World->GetTimerManager().SetTimer(ShadowEffectTailTimer,
					FTimerDelegate::CreateWeakLambda(StoppedShadowEffect.Get(), [Effect]()
					{
						if (Effect.IsValid())
						{
							Effect->DestroyComponent();
						}
					}), ShadowEffectTailDuration, false);
			}
			else
			{
				ShadowEffect->DestroyComponent();
			}
		}
	}
	ShadowEffect = nullptr;
}

//-----------------------------------------------------
//元のサイズで戻れる場所か
//-----------------------------------------------------
bool UPlayerShadowState::CanRestoreAtFeet(const FVector& Feet) const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UWorld* World = GetWorld();
	const UCapsuleComponent* Capsule = PlayerCharacter ? PlayerCharacter->GetCapsuleComponent() : nullptr;
	if (!World || !IsValid(Capsule))
	{
		return false;
	}
	const float Scale = Capsule->GetScaledCapsuleHalfHeight() / FMath::Max(1.f, Capsule->GetUnscaledCapsuleHalfHeight());
	const float HalfHeight = OriginalCapsuleHalfHeight * Scale;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerCharacter);
	return !World->OverlapBlockingTestByProfile(Feet + FVector(0.f, 0.f, HalfHeight), Capsule->GetComponentQuat(), OriginalCollisionProfile,
												FCollisionShape::MakeCapsule(OriginalCapsuleRadius * Scale, HalfHeight), Params);
}
