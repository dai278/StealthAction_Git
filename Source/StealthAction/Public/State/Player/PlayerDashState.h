#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerDashState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerDashState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerDashState();
};
