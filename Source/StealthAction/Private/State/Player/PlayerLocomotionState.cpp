#include "State/Player/PlayerLocomotionState.h"
#include "State/Player/PlayerStateMachine.h"

UPlayerLocomotionState::UPlayerLocomotionState()
{
	bAllowMovementInput = true;
	FootstepNoiseLevel = 1;
}

void UPlayerLocomotionState::OnUpdate_Implementation(float DeltaTime)
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->RefreshLocomotionState(); }
}
