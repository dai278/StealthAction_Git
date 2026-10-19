#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerDeadState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerDeadState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerDeadState();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	bool bOpenGameOverLevel = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death", meta = (EditCondition = "bOpenGameOverLevel"))
	FName GameOverLevel = TEXT("GameOver");
	virtual void OnEnter_Implementation() override;

};
