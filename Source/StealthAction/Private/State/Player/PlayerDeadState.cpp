#include "State/Player/PlayerDeadState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerDeadState::UPlayerDeadState() { PlayerStatus = EPlayerStatus::Dead; }

void UPlayerDeadState::OnEnter_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (PlayerCharacter->m_bDash) { PlayerCharacter->EndDash(); }
		PlayerCharacter->m_bCanControl = false;
		PlayerCharacter->m_playerInfo.isAlive = false;
	}
}
