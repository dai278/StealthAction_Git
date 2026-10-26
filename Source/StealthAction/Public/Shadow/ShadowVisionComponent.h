/*━━━━━━━━━
@file ShadowVisionComponent.h
@brief 影状態の間、画面周辺を暗くして視野を狭めるコンポーネント
@author 24CU0110 大川翔生
@date 2026/10/27
最終更新日 2026/10/27
━━━━━━━━━*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShadowVisionComponent.generated.h"

//前方宣言
class UCameraComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STEALTHACTION_API UShadowVisionComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UShadowVisionComponent();

	//更新処理
	virtual void TickComponent(
		float _deltaTime,								//経過時間
		ELevelTick _tickType,							//更新の種類
		FActorComponentTickFunction* _pThisTickFunction	//更新関数
	) override;

protected:
	//ゲーム開始時の処理
	virtual void BeginPlay() override;

	//終了時の処理
	virtual void EndPlay(const EEndPlayReason::Type _endPlayReason) override;

private:
	//影に入った通知
	UFUNCTION()
	void HandleEnteredShadow();

	//影から出た通知
	UFUNCTION()
	void HandleExitedShadow();

	//目標の強さを設定して更新を開始
	//_targetAlpha：目標の強さ(0で効果なし、1で最大)
	void SetTargetAlpha(float _targetAlpha);

	//強さをカメラとマテリアルへ反映
	//_alpha：効果の強さ(0～1)
	void ApplyAlpha(float _alpha);

	//効果をカメラから完全に外す
	void ReleaseEffect();

private :
	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> m_pCamera = nullptr;				//効果を載せるカメラ

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> m_pVisionMID = nullptr;	//演出用の動的マテリアル

	float m_alpha = k_alphaMin;			//現在の強さ(0～1)
	float m_targetAlpha = k_alphaMin;	//目標の強さ
	bool m_bBlendableAdded = false;		//カメラへマテリアルを追加済みか

protected:
	UPROPERTY(EditAnywhere, Category = "ShadowVision")
	TObjectPtr<UMaterialInterface> m_pVisionMaterial = nullptr;	//視野狭窄用のポストプロセスマテリアル(未設定なら標準ビネットで代用)

	UPROPERTY(EditAnywhere, Category = "ShadowVision", meta = (ClampMin = "0.0", Units = "s"))
	float m_blendInTime = k_defaultBlendInTime;		//影に入ってから効果が最大になるまでの時間

	UPROPERTY(EditAnywhere, Category = "ShadowVision", meta = (ClampMin = "0.0", Units = "s"))
	float m_blendOutTime = k_defaultBlendOutTime;	//影を出てから効果が消えるまでの時間

	UPROPERTY(EditAnywhere, Category = "ShadowVision")
	FName m_strengthParameterName = TEXT("Strength");	//マテリアル側の強さパラメータ名

	UPROPERTY(EditAnywhere, Category = "ShadowVision|Fallback", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float m_vignetteIntensity = k_defaultVignetteIntensity;	//標準ビネットの最大強度

	UPROPERTY(EditAnywhere, Category = "ShadowVision|Fallback", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float m_saturation = k_defaultSaturation;		//影中の彩度(1で変化なし)

private:
	static constexpr float k_alphaMin = 0.f;					//効果なしの強さ
	static constexpr float k_alphaMax = 1.f;					//効果最大の強さ
	static constexpr float k_minBlendTime = 0.01f;				//ブレンド時間の下限(0除算防止)
	static constexpr float k_easeExponent = 2.f;				//イージングの強さ
	static constexpr float k_defaultBlendInTime = 0.4f;			//入る時のブレンド時間(秒)
	static constexpr float k_defaultBlendOutTime = 0.3f;		//出る時のブレンド時間(秒)
	static constexpr float k_defaultVignetteIntensity = 0.9f;	//標準ビネットの最大強度
	static constexpr float k_defaultSaturation = 0.5f;			//影中の彩度
};
