#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerCrouchState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerCrouchState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerCrouchState();
};
