#include "State/Player/PlayerAttackState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerAttackState::UPlayerAttackState() { PlayerStatus = EPlayerStatus::Attack; }

void UPlayerAttackState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (Player->m_bDash) { Player->EndDash(); }
		Player->m_bCanControl = false;
		Player->m_bCanAttack = true;
	}
}

void UPlayerAttackState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (!Player->m_bCanAttack) { RequestPlayerState(EPlayerStatus::Idle); }
	}
}

void UPlayerAttackState::OnExit_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->FinishAttackState(); }
}
