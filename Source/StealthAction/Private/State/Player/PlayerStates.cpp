#include "State/Player/PlayerStates.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UPlayerIdleState::UPlayerIdleState() { PlayerStatus = EPlayerStatus::Idle; }
UPlayerWalkState::UPlayerWalkState() { PlayerStatus = EPlayerStatus::Walk; }
UPlayerDashState::UPlayerDashState() { PlayerStatus = EPlayerStatus::Dash; }
UPlayerJumpState::UPlayerJumpState() { PlayerStatus = EPlayerStatus::Jump; }
UPlayerCrouchState::UPlayerCrouchState() { PlayerStatus = EPlayerStatus::Crouch; }
UPlayerAttackState::UPlayerAttackState() { PlayerStatus = EPlayerStatus::Attack; }
UPlayerDamageState::UPlayerDamageState() { PlayerStatus = EPlayerStatus::Damage; }
UPlayerDeadState::UPlayerDeadState() { PlayerStatus = EPlayerStatus::Dead; }
UPlayerShadowState::UPlayerShadowState() { PlayerStatus = EPlayerStatus::InShadow; }
UPlayerInteractState::UPlayerInteractState() { PlayerStatus = EPlayerStatus::Interact; }
UPlayerInteractAnimationState::UPlayerInteractAnimationState() { PlayerStatus = EPlayerStatus::InteractAnimation; }

void UPlayerLocomotionState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->RefreshLocomotionState(); }
}

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

void UPlayerDeadState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (Player->m_bDash) { Player->EndDash(); }
		Player->m_bCanControl = false;
		Player->m_playerInfo.isAlive = false;
	}
}

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

void UPlayerInteractState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (Player->m_bDash) { Player->EndDash(); }
		Player->m_bCanControl = false;
		Player->m_interactTimer = 0.f;
	}
}

void UPlayerInteractState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->UpdateInteract(DeltaTime); }
}

void UPlayerInteractState::OnExit_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->m_bCanControl = true; }
}

void UPlayerInteractAnimationState::OnEnter_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		if (Player->m_bDash) { Player->EndDash(); }
		Player->m_bCanControl = false;
		Player->m_interactTimer = 0.f;
		Player->m_bInteractionExecuted = false;
	}
}

void UPlayerInteractAnimationState::OnUpdate_Implementation(float DeltaTime)
{
	if (APlayerCharacter* Player = GetPlayer()) { Player->UpdateInteract(DeltaTime); }
}

void UPlayerInteractAnimationState::OnExit_Implementation()
{
	if (APlayerCharacter* Player = GetPlayer())
	{
		Player->m_bCanControl = true;
		Player->m_interactTimer = 0.f;
	}
}
