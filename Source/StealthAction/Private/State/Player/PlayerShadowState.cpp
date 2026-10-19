#include "State/Player/PlayerShadowState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerShadowState::UPlayerShadowState() { PlayerStatus = EPlayerStatus::InShadow; }

void UPlayerShadowState::OnEnter_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->EnterShadowState(); }
}

void UPlayerShadowState::OnExit_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->ExitShadowState(); }
}

void UPlayerShadowState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->UpdateShadow(DeltaTime); }
}
