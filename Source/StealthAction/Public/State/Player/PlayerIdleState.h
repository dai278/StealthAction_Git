#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerIdleState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerIdleState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerIdleState();
};
