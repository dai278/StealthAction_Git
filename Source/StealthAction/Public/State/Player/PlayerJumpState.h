#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerJumpState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerJumpState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerJumpState();
};
