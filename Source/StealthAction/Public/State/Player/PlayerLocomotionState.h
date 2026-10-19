#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerLocomotionState.generated.h"

// Shared locomotion selection; movement itself still uses Enhanced Input.
UCLASS(Abstract, Blueprintable)
class STEALTHACTION_API UPlayerLocomotionState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerLocomotionState();
protected:
	virtual void OnUpdate_Implementation(float DeltaTime) override;
};
