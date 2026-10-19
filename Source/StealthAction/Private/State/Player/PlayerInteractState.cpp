#include "State/Player/PlayerInteractState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerInteractState::UPlayerInteractState() { PlayerStatus = EPlayerStatus::Interact; }

void UPlayerInteractState::OnEnter_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (PlayerCharacter->m_bDash) { PlayerCharacter->EndDash(); }
		PlayerCharacter->m_bCanControl = false;
		PlayerCharacter->m_interactTimer = 0.f;
	}
}

void UPlayerInteractState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->UpdateInteract(DeltaTime); }
}

void UPlayerInteractState::OnExit_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->m_bCanControl = true; }
}
