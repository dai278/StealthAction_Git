#include "State/Player/PlayerLocomotionState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

void UPlayerLocomotionState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->RefreshLocomotionState(); }
}
