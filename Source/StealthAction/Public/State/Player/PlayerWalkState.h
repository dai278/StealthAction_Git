#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerWalkState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerWalkState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerWalkState();

protected:

};
