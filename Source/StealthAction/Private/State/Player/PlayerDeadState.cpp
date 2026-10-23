#include "State/Player/PlayerDeadState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerDeadState::UPlayerDeadState()
{
	PlayerStatus = EPlayerStatus::Dead;
	MoveSpeed = 0.f;
	bAllowManualControl = false;
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerDeadState::OnEnter_Implementation()
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->CancelDash(); }
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		PlayerCharacter->m_playerInfo.isAlive = false;
		PlayerCharacter->m_playerInfo.bIsGetKeyItem = 0;
		if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement()) { Movement->StopMovementImmediately(); }
		if (bOpenGameOverLevel && !GameOverLevel.IsNone() && GetWorld()) { UGameplayStatics::OpenLevel(PlayerCharacter, GameOverLevel); }
	}
}
