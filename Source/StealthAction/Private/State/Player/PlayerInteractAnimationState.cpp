#include "State/Player/PlayerInteractAnimationState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerInteractAnimationState::UPlayerInteractAnimationState() { PlayerStatus = EPlayerStatus::InteractAnimation; }

void UPlayerInteractAnimationState::OnEnter_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (PlayerCharacter->m_bDash) { PlayerCharacter->EndDash(); }
		PlayerCharacter->m_bCanControl = false;
		PlayerCharacter->m_interactTimer = 0.f;
		PlayerCharacter->m_bInteractionExecuted = false;
	}
}

void UPlayerInteractAnimationState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->UpdateInteract(DeltaTime); }
}

void UPlayerInteractAnimationState::OnExit_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		PlayerCharacter->m_bCanControl = true;
		PlayerCharacter->m_interactTimer = 0.f;
	}
}
