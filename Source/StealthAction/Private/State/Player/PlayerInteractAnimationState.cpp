#include "State/Player/PlayerInteractAnimationState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerInteractAnimationState::UPlayerInteractAnimationState() { PlayerStatus = EPlayerStatus::InteractAnimation; }

void UPlayerInteractAnimationState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (Player->m_bDash) { Player->EndDash(); }
		Player->m_bCanControl = false;
		Player->m_interactTimer = 0.f;
		Player->m_bInteractionExecuted = false;
	}
}

void UPlayerInteractAnimationState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->UpdateInteract(DeltaTime); }
}

void UPlayerInteractAnimationState::OnExit_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		Player->m_bCanControl = true;
		Player->m_interactTimer = 0.f;
	}
}
