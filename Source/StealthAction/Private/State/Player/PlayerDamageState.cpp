#include "State/Player/PlayerDamageState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerDamageState::UPlayerDamageState() { PlayerStatus = EPlayerStatus::Damage; }

void UPlayerDamageState::OnEnter_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (PlayerCharacter->m_bDash) { PlayerCharacter->EndDash(); }
		PlayerCharacter->m_bCanControl = false;
	}
}

void UPlayerDamageState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (m_stateTimer >= PlayerCharacter->m_damageTime) { RequestPlayerState(EPlayerStatus::Idle); }
	}
}

void UPlayerDamageState::OnExit_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		PlayerCharacter->m_knockBackVelocity = FVector::ZeroVector;
		PlayerCharacter->m_bCanControl = true;
	}
}
