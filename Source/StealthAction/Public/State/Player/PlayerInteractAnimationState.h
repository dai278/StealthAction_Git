//----------------------------------------------------------
//概要：インタラクト実行ステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerInteractAnimationState.generated.h"

UCLASS(Blueprintable)

class STEALTHACTION_API UPlayerInteractAnimationState : public UPlayerStateBase
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	UPlayerInteractAnimationState();

	//対象なしでアニメーションだけ開始しない
	virtual bool CanEnter() const override;

	//対象を保存してインタラクト開始
	bool BeginInteraction(class AInteract* Target);

	//インタラクト実行
	void CompleteInteraction();

  protected:

	//インタラクト時に再生するモンタージュ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Animation")
	TObjectPtr<class UAnimMontage> InteractionMontage = nullptr;

	//モンタージュの再生速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Animation", meta = (ClampMin = "0.01"))
	float MontagePlayRate = 1.f;

	//中断時にモンタージュを止めるまでの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Animation", meta = (ClampMin = "0.0", Units = "s"))
	float MontageBlendOutTime = 0.15f;

	//モンタージュ未設定時のインタラクト実行までの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "0.0"))
	float InteractionDuration = 2.5f;

	//時間ではなくアニメーション通知を待つか
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bWaitForAnimationNotify = false;

	//開始処理
	virtual void OnEnter_Implementation() override;

	//更新処理
	virtual void OnUpdate_Implementation(float DeltaTime) override;

	//終了処理
	virtual void OnExit_Implementation() override;

  private:

	//実行対象が現在も使用可能で範囲内か
	bool IsTargetAvailable() const;

	//モンタージュ終了通知(ここでは結果だけ保存する)
	void OnInteractionMontageEnded(class UAnimMontage* Montage, bool bInterrupted);

	//再生したAnimInstanceの保存用
	TWeakObjectPtr<class UAnimInstance> PlayingAnimInstance;

	//モンタージュを再生できたか
	bool bMontageStarted = false;

	//モンタージュが終了したか
	bool bMontageEnded = false;

	//再生失敗や中断があったか
	bool bMontageCancelled = false;

	//インタラクト対象の保存用
	TWeakObjectPtr<class AInteract> InteractionTarget;

	//インタラクトを実行済みか
	bool bInteractionExecuted = false;
};
