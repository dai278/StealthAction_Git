#include "State/Player/PlayerInteractAnimationState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Interact/Interact.h"

UPlayerInteractAnimationState::UPlayerInteractAnimationState()
{
	PlayerStatus = EPlayerStatus::InteractAnimation;
	MoveSpeed = 0.f;
	bAllowManualControl = false;
}

bool UPlayerInteractAnimationState::BeginInteraction(AInteract* Target)
{
	InteractionTarget = Target;
	const bool bStarted = RequestPlayerState(EPlayerStatus::InteractAnimation);
	if (!bStarted) { InteractionTarget.Reset(); }
	return bStarted;
}

bool UPlayerInteractAnimationState::CanEnterState_Implementation() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	return InteractionTarget.IsValid() || (PlayerCharacter && IsValid(PlayerCharacter->m_hitInteractOb));
}

void UPlayerInteractAnimationState::OnEnter_Implementation()
{
	bInteractionExecuted = false;
	if (!InteractionTarget.IsValid())
	{
		if (APlayerCharacter* PlayerCharacter = GetPlayer()) { InteractionTarget = PlayerCharacter->m_hitInteractOb; }
	}
}

void UPlayerInteractAnimationState::OnUpdate_Implementation(float DeltaTime)
{
	if (!InteractionTarget.IsValid())
	{
		if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->ReturnToLocomotion(); }
		return;
	}
	if (bInteractionExecuted || (!bWaitForAnimationNotify && m_stateTimer >= FMath::Max(0.f, InteractionDuration))) { CompleteInteraction(); }
}

void UPlayerInteractAnimationState::CompleteInteraction()
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UPlayerStateMachine* Machine = GetPlayerMachine();
	if (!PlayerCharacter || !Machine || Machine->GetPlayerStatus() != EPlayerStatus::InteractAnimation) { return; }
	if (!bInteractionExecuted)
	{
		bInteractionExecuted = true;
		if (InteractionTarget.IsValid()) { InteractionTarget->Interact(PlayerCharacter); }
	}
	if (IsValid(Machine) && Machine->GetPlayerStatus() == EPlayerStatus::InteractAnimation) { Machine->ReturnToLocomotion(); }
}

void UPlayerInteractAnimationState::OnExit_Implementation() { InteractionTarget.Reset(); }
