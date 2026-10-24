//----------------------------------------------------------
//概要：影ステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "Engine/EngineTypes.h"
#include "PlayerShadowState.generated.h"

UCLASS(Blueprintable)

class STEALTHACTION_API UPlayerShadowState : public UPlayerStateBase
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	UPlayerShadowState();

  protected:

	//影状態のコリジョン設定
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	FName ShadowCollisionProfile = TEXT("PlayerInShadow");

	//影状態のカプセル半径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Collision", meta = (ClampMin = "1.0", Units = "cm"))
	float ShadowCapsuleRadius = 10.f;

	//影状態のカプセル半高さ(半径より小さくはできない)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Collision", meta = (ClampMin = "1.0", Units = "cm"))
	float ShadowCapsuleHalfHeight = 10.f;

	//影エフェクトのNiagara
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	TObjectPtr<class UNiagaraSystem> ShadowEffectSystem = nullptr;

	//足元からのエフェクトの位置調整
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Effect")
	FVector ShadowEffectOffset = FVector(0.f, 0.f, 2.f);

	//潜る進行度を渡すNiagaraのパラメータ名
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Effect")
	FName ShadowAlphaParameterName = TEXT("User.ShadowAlpha");

	//影に潜るまでの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Appearance", meta = (ClampMin = "0.0", Units = "s"))
	float SinkDuration = 0.4f;

	//メッシュを下げる距離
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Appearance", meta = (ClampMin = "0.0", Units = "cm"))
	float SinkDistance = 100.f;

	//マテリアル側の不透明度パラメータ名
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Appearance")
	FName OpacityParameterName = TEXT("Opacity");

	//開始条件
	virtual bool CanEnterState_Implementation() const override;

	//開始処理
	virtual void OnEnter_Implementation() override;

	//更新処理
	virtual void OnUpdate_Implementation(float DeltaTime) override;

	//終了処理
	virtual void OnExit_Implementation() override;

  private:

	//元のサイズで戻れる場所か
	bool CanRestoreAtFeet(const FVector& Feet) const;

	//変更前のカプセルサイズ
	float OriginalCapsuleRadius = 0.f;

	//変更前のカプセル半高さ
	float OriginalCapsuleHalfHeight = 0.f;

	//最後に元のサイズで戻れた足元位置
	FVector LastSafeFeet = FVector::ZeroVector;

	//縮小時の位置補正後のメッシュ位置
	FVector SinkStartMeshLocation = FVector::ZeroVector;

	//生成した影エフェクト
	UPROPERTY(Transient)
	TObjectPtr<class UNiagaraComponent> ShadowEffect = nullptr;

	//影エフェクト停止
	void StopShadowEffect();

	//移動は止めずにメッシュだけ影に沈める
	void UpdateSinkAppearance(float DeltaTime);

	//変更前のメッシュ位置
	FVector OriginalMeshLocation = FVector::ZeroVector;

	//潜り始めてからの時間
	float SinkElapsed = 0.f;

	//解除時に元へ戻すためのマテリアル
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UMaterialInterface>> OriginalMaterials;

	//潜る演出用のマテリアル
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UMaterialInstanceDynamic>> SinkMaterials;

	//各マテリアルの変更前の不透明度
	TArray<float> OriginalOpacities;

	//変更前のメッシュの表示状態
	bool bOriginalMeshVisible = true;

	//変更前のコリジョン設定
	FName OriginalCollisionProfile = NAME_None;

	//変更前のメッシュの衝突設定
	ECollisionEnabled::Type OriginalMeshCollision = ECollisionEnabled::QueryOnly;

	//変更前のオーバーラップ設定
	bool bOriginalOverlapEvents = false;

	//影状態の設定を反映済みか
	bool bShadowApplied = false;
};
