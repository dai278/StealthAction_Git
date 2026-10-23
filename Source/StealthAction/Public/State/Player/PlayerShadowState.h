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
	//影エフェクトのクラス
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	TSubclassOf<AActor> ShadowEffectClass;
	//影に潜るまでの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Appearance", meta = (ClampMin = "0.0", Units = "s"))
	float SinkDuration = 0.4f;
	//メッシュを下げる距離
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Appearance", meta = (ClampMin = "0.0", Units = "cm"))
	float SinkDistance = 100.f;
	//マテリアル側の不透明度パラメータ名
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow|Appearance")
	FName OpacityParameterName = TEXT("Opacity");
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
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
