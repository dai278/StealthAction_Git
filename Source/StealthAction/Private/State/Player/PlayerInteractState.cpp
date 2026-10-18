#include "State/Player/PlayerInteractState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerInteractState::UPlayerInteractState() { PlayerStatus = EPlayerStatus::Interact; }

void UPlayerInteractState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (Player->m_bDash) { Player->EndDash(); }
		Player->m_bCanControl = false;
		Player->m_interactTimer = 0.f;
	}
}

void UPlayerInteractState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->UpdateInteract(DeltaTime); }
}

void UPlayerInteractState::OnExit_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->m_bCanControl = true; }
}
