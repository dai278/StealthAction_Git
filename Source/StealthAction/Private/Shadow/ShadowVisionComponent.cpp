/*━━━━━━━━━
@file ShadowVisionComponent.cpp
@brief 影状態の間、画面周辺を暗くして視野を狭めるコンポーネント
@author 24CU0110 大川翔生
@date 2026/10/27
最終更新日 2026/10/27
━━━━━━━━━*/

#include "Shadow/ShadowVisionComponent.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/Scene.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UShadowVisionComponent::UShadowVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//効果が動いている間だけ更新する
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

//-----------------------------------------------------
//ゲーム開始時の処理
//-----------------------------------------------------
void UShadowVisionComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerCharacter* pPlayer = Cast<APlayerCharacter>(GetOwner());
	if (!IsValid(pPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Owner is not APlayerCharacter."), *GetName());
		return;
	}

	m_pCamera = pPlayer->FindComponentByClass<UCameraComponent>();
	if (!IsValid(m_pCamera))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Camera component not found."), *GetName());
	}

	//影の出入り通知を受け取る
	pPlayer->OnEnteredShadow.AddUniqueDynamic(this, &UShadowVisionComponent::HandleEnteredShadow);
	pPlayer->OnExitedShadow.AddUniqueDynamic(this, &UShadowVisionComponent::HandleExitedShadow);

	if (IsValid(m_pVisionMaterial))
	{
		m_pVisionMID = UMaterialInstanceDynamic::Create(m_pVisionMaterial, this);
	}
}

//-----------------------------------------------------
//終了時の処理
//-----------------------------------------------------
void UShadowVisionComponent::EndPlay(const EEndPlayReason::Type _endPlayReason)
{
	if (APlayerCharacter* pPlayer = Cast<APlayerCharacter>(GetOwner()))
	{
		pPlayer->OnEnteredShadow.RemoveDynamic(this, &UShadowVisionComponent::HandleEnteredShadow);
		pPlayer->OnExitedShadow.RemoveDynamic(this, &UShadowVisionComponent::HandleExitedShadow);
	}

	ReleaseEffect();
	Super::EndPlay(_endPlayReason);
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UShadowVisionComponent::TickComponent(
	float _deltaTime,
	ELevelTick _tickType,
	FActorComponentTickFunction* _pThisTickFunction)
{
	Super::TickComponent(_deltaTime, _tickType, _pThisTickFunction);

	//入る時と出る時で別の時間を使う
	const float blendTime = FMath::Max((m_targetAlpha > m_alpha) ? m_blendInTime : m_blendOutTime, k_minBlendTime);
	m_alpha = FMath::FInterpConstantTo(m_alpha, m_targetAlpha, _deltaTime, k_alphaMax / blendTime);

	ApplyAlpha(m_alpha);

	//目標に着いたら更新を止める
	if (FMath::IsNearlyEqual(m_alpha, m_targetAlpha))
	{
		SetComponentTickEnabled(false);

		//完全に消えたらカメラから外す
		if (m_alpha <= k_alphaMin) { ReleaseEffect(); }
	}
}

//-----------------------------------------------------
//影に入った通知
//-----------------------------------------------------
void UShadowVisionComponent::HandleEnteredShadow()
{
	SetTargetAlpha(k_alphaMax);
}

//-----------------------------------------------------
//影から出た通知
//-----------------------------------------------------
void UShadowVisionComponent::HandleExitedShadow()
{
	SetTargetAlpha(k_alphaMin);
}

//-----------------------------------------------------
//目標の強さを設定して更新を開始
//-----------------------------------------------------
void UShadowVisionComponent::SetTargetAlpha(float _targetAlpha)
{
	m_targetAlpha = _targetAlpha;
	SetComponentTickEnabled(true);
}

//-----------------------------------------------------
//強さをカメラとマテリアルへ反映
//-----------------------------------------------------
void UShadowVisionComponent::ApplyAlpha(float _alpha)
{
	if (!IsValid(m_pCamera)) { return; }

	const float easedAlpha = FMath::InterpEaseInOut(k_alphaMin, k_alphaMax, _alpha, k_easeExponent);
	FPostProcessSettings& settings = m_pCamera->PostProcessSettings;

	//専用マテリアルがあれば筒状の視野狭窄
	if (IsValid(m_pVisionMID))
	{
		if (!m_bBlendableAdded)
		{
			settings.AddBlendable(m_pVisionMID.Get(), k_alphaMax);
			m_bBlendableAdded = true;
		}

		m_pVisionMID->SetScalarParameterValue(m_strengthParameterName, easedAlpha);
		return;
	}

	//無ければ標準のビネットと彩度低下で代用
	const float saturation = FMath::Lerp(k_alphaMax, m_saturation, easedAlpha);
	settings.bOverride_VignetteIntensity = true;
	settings.VignetteIntensity = FMath::Lerp(k_alphaMin, m_vignetteIntensity, easedAlpha);
	settings.bOverride_ColorSaturation = true;
	settings.ColorSaturation = FVector4(saturation, saturation, saturation, k_alphaMax);
}

//-----------------------------------------------------
//効果をカメラから完全に外す
//-----------------------------------------------------
void UShadowVisionComponent::ReleaseEffect()
{
	m_alpha = k_alphaMin;
	if (!IsValid(m_pCamera)) { return; }

	FPostProcessSettings& settings = m_pCamera->PostProcessSettings;
	if (m_bBlendableAdded && IsValid(m_pVisionMID))
	{
		settings.RemoveBlendable(m_pVisionMID.Get());
	}

	m_bBlendableAdded = false;
	settings.bOverride_VignetteIntensity = false;
	settings.bOverride_ColorSaturation = false;
}