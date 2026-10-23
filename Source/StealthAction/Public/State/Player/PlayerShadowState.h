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
	//影状態のメッシュ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	TObjectPtr<class USkeletalMesh> ShadowMesh = nullptr;
	//影エフェクトのクラス
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	TSubclassOf<AActor> ShadowEffectClass;
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	//変更前のメッシュ
	UPROPERTY(Transient)
	TObjectPtr<class USkeletalMesh> OriginalMesh = nullptr;
	//変更前のコリジョン設定
	FName OriginalCollisionProfile = NAME_None;
	//変更前のメッシュの衝突設定
	ECollisionEnabled::Type OriginalMeshCollision = ECollisionEnabled::QueryOnly;
	//変更前のオーバーラップ設定
	bool bOriginalOverlapEvents = false;
	//影状態の設定を反映済みか
	bool bShadowApplied = false;
};
