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
	//ゲームオーバー画面へ移動するか
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	bool bOpenGameOverLevel = true;
	//ゲームオーバーのレベル名
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death", meta = (EditCondition = "bOpenGameOverLevel"))
	FName GameOverLevel = TEXT("GameOver");
	//ゲームオーバー前の暗転時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade", meta = (ClampMin = "0.0", Units = "s", EditCondition = "bOpenGameOverLevel"))
	float FadeOutDuration = 0.5f;
	//暗転後に待つ時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade", meta = (ClampMin = "0.0", Units = "s", EditCondition = "bOpenGameOverLevel"))
	float BlackHoldDuration = 0.1f;
	//移動先で明るく戻す時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade", meta = (ClampMin = "0.0", Units = "s", EditCondition = "bOpenGameOverLevel"))
	float FadeInDuration = 0.5f;
	//フェードの色
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death|Fade", meta = (EditCondition = "bOpenGameOverLevel"))
	FLinearColor FadeColor = FLinearColor::Black;
	virtual void OnEnter_Implementation() override;

};
