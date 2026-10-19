#include "State/Player/PlayerAttackState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerAttackState::UPlayerAttackState() { PlayerStatus = EPlayerStatus::Attack; }

void UPlayerAttackState::OnEnter_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (PlayerCharacter->m_bDash) { PlayerCharacter->EndDash(); }
		PlayerCharacter->m_bCanControl = false;
		PlayerCharacter->m_bCanAttack = true;
	}
}

void UPlayerAttackState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (!PlayerCharacter->m_bCanAttack) { RequestPlayerState(EPlayerStatus::Idle); }
	}
}

void UPlayerAttackState::OnExit_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->FinishAttackState(); }
}
