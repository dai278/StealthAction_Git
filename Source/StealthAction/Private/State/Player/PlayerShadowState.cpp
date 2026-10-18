#include "State/Player/PlayerShadowState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerShadowState::UPlayerShadowState() { PlayerStatus = EPlayerStatus::InShadow; }

void UPlayerShadowState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->EnterShadowState(); }
}

void UPlayerShadowState::OnExit_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->ExitShadowState(); }
}

void UPlayerShadowState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->UpdateShadow(DeltaTime); }
}
