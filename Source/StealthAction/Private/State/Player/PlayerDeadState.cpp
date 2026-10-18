#include "State/Player/PlayerDeadState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerDeadState::UPlayerDeadState() { PlayerStatus = EPlayerStatus::Dead; }

void UPlayerDeadState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (Player->m_bDash) { Player->EndDash(); }
		Player->m_bCanControl = false;
		Player->m_playerInfo.isAlive = false;
	}
}
