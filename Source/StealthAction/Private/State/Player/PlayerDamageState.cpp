#include "State/Player/PlayerDamageState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerDamageState::UPlayerDamageState() { PlayerStatus = EPlayerStatus::Damage; }

void UPlayerDamageState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (Player->m_bDash) { Player->EndDash(); }
		Player->m_bCanControl = false;
	}
}

void UPlayerDamageState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (m_stateTimer >= Player->m_damageTime) { RequestPlayerState(EPlayerStatus::Idle); }
	}
}

void UPlayerDamageState::OnExit_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		Player->m_knockBackVelocity = FVector::ZeroVector;
		Player->m_bCanControl = true;
	}
}
