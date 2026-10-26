//担当日髙
//----------------------------------------------------------
//概要：死亡ステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerDeadState.generated.h"

UCLASS(Blueprintable)

class STEALTHACTION_API UPlayerDeadState : public UPlayerStateBase
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	UPlayerDeadState();

  protected:

	//死亡時に再生するモンタージュ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Animation")
	TObjectPtr<class UAnimMontage> DeathMontage = nullptr;

	//死亡モンタージュの再生速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Animation", meta = (ClampMin = "0.01"))
	float MontagePlayRate = 1.f;

	//死亡アニメーションの長さだけ待ってから暗転するか
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Animation")
	bool bWaitForDeathMontage = false;

	//暗転開始までの追加の待ち時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade", meta = (ClampMin = "0.0", Units = "s"))
	float GameOverDelay = 0.f;

	//ゲームオーバー画面へ移動するか
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	bool bOpenGameOverLevel = true;

	//ゲームオーバーのレベル名
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death", meta = (EditCondition = "bOpenGameOverLevel"))
	FName GameOverLevel = TEXT("GameOver");

	//ゲームオーバー前の暗転時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade",
			  meta = (ClampMin = "0.0", Units = "s", EditCondition = "bOpenGameOverLevel"))
	float FadeOutDuration = 0.5f;

	//暗転後に待つ時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade",
			  meta = (ClampMin = "0.0", Units = "s", EditCondition = "bOpenGameOverLevel"))
	float BlackHoldDuration = 0.1f;

	//移動先で明るく戻す時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade",
			  meta = (ClampMin = "0.0", Units = "s", EditCondition = "bOpenGameOverLevel"))
	float FadeInDuration = 0.5f;

	//フェードの色
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade", meta = (EditCondition = "bOpenGameOverLevel"))
	FLinearColor FadeColor = FLinearColor::Black;

	//開始処理
	virtual void OnEnter_Implementation() override;

	//更新処理
	virtual void OnUpdate_Implementation(float DeltaTime) override;

	//終了処理
	virtual void OnExit_Implementation() override;

  private:

	//暗転とゲームオーバーへの移動を1回だけ開始する
	void BeginGameOverTransition();

	//再生したAnimInstanceの保存用
	TWeakObjectPtr<class UAnimInstance> PlayingAnimInstance;

	//暗転を開始する時間
	float FadeStartTime = 0.f;

	//画面遷移を要求済みか
	bool bTransitionRequested = false;
};
