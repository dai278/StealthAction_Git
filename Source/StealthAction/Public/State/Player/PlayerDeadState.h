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
	virtual void OnEnter_Implementation() override;
};
