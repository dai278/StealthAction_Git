#include "State/Player/PlayerStateMachine.h"
#include "State/Player/PlayerStateBase.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

void UPlayerStateMachine::Initialize(AActor* Owner)
{
	Super::Initialize(Owner);
	PlayerStates.Reset();
	bConfigured = false;
	const APlayerCharacter* Player = Cast<APlayerCharacter>(Owner);
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStateMachine requires a PlayerCharacter owner."));
		return;
	}
	InitialStatus = Player->m_status;
	bool bValid = true;
	for (UStateBase* State : GetRegisteredStates())
	{
		UPlayerStateBase* PlayerState = Cast<UPlayerStateBase>(State);
		if (!PlayerState || PlayerStates.Contains(PlayerState->GetPlayerStatus()))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: State Classes must contain unique player statuses and only PlayerStateBase subclasses."), *GetName());
			bValid = false;
			continue;
		}
		PlayerStates.Add(PlayerState->GetPlayerStatus(), PlayerState);
	}

	// Existing character inputs rely on all of these statuses. Fail configuration
	// explicitly rather than silently applying half a transition's side effects.
	const EPlayerStatus RequiredStatuses[] = {
		EPlayerStatus::Idle, EPlayerStatus::Walk, EPlayerStatus::Dash,
		EPlayerStatus::Jump, EPlayerStatus::Crouch, EPlayerStatus::Attack,
		EPlayerStatus::Damage, EPlayerStatus::Dead, EPlayerStatus::InShadow,
		EPlayerStatus::Interact, EPlayerStatus::InteractAnimation
	};
	for (EPlayerStatus Status : RequiredStatuses)
	{
		if (!PlayerStates.Contains(Status))
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Missing State Classes entry for EPlayerStatus %d."), *GetName(), static_cast<int32>(Status));
			bValid = false;
		}
	}
	bConfigured = bValid;
	if (bConfigured && !ChangePlayerState(InitialStatus))
	{
		bConfigured = false;
		UE_LOG(LogTemp, Error, TEXT("%s: Initial player state refused entry."), *GetName());
	}
}

bool UPlayerStateMachine::ChangePlayerState(EPlayerStatus NewStatus)
{
	if (!bConfigured || bTransitionInProgress) { return false; }
	TGuardValue<bool> TransitionGuard(bTransitionInProgress, true);
	const TObjectPtr<UPlayerStateBase>* State = PlayerStates.Find(NewStatus);
	if (!State) { return false; }
	const UPlayerStateBase* Current = Cast<UPlayerStateBase>(GetCurrentState());
	if (Current && Current->GetPlayerStatus() == EPlayerStatus::Dead) { return false; }
	// Damage and death must interrupt attack/interaction even if a BP locks exit.
	if (NewStatus == EPlayerStatus::Damage || NewStatus == EPlayerStatus::Dead)
	{
		return ForceChangeState(State->Get());
	}
	return ChangeState(State->Get());
}

EPlayerStatus UPlayerStateMachine::GetPlayerStatus() const
{
	const UPlayerStateBase* State = Cast<UPlayerStateBase>(GetCurrentState());
	return State ? State->GetPlayerStatus() : InitialStatus;
}

bool UPlayerStateMachine::IsLocomotionState() const
{
	if (!bConfigured || !GetCurrentState()) { return false; }
	switch (GetPlayerStatus())
	{
	case EPlayerStatus::Idle:
	case EPlayerStatus::Walk:
	case EPlayerStatus::Dash:
	case EPlayerStatus::Jump:
	case EPlayerStatus::Crouch:
		return true;
	default:
		return false;
	}
}

void UPlayerStateMachine::Stop()
{
	if (bTransitionInProgress) { return; }
	TGuardValue<bool> TransitionGuard(bTransitionInProgress, true);
	Super::Stop();
	bConfigured = false;
}
