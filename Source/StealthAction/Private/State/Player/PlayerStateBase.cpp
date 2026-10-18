#include "State/Player/PlayerStateBase.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

UWorld* UPlayerStateBase::GetWorld() const
{
	return IsValid(Player) ? Player->GetWorld() : nullptr;
}

void UPlayerStateBase::Initialize(UStateMachine* StateMachine, AActor* Owner)
{
	Super::Initialize(StateMachine, Owner);
	Player = Cast<APlayerCharacter>(Owner);
}

void UPlayerStateBase::Enter()
{
	Super::Enter();
	OnEnter();
}

void UPlayerStateBase::Update(const float& DeltaTime)
{
	Super::Update(DeltaTime);
	OnUpdate(DeltaTime);
}

void UPlayerStateBase::Exit()
{
	OnExit();
	Super::Exit();
}

bool UPlayerStateBase::CanEnter() const
{
	return IsValid(Player) && Super::CanEnter() && CanEnterState();
}

bool UPlayerStateBase::CanExit() const
{
	return Super::CanExit() && CanExitState();
}

bool UPlayerStateBase::CanTransitionTo(const UStateBase* NewState) const
{
	const UPlayerStateBase* PlayerState = Cast<UPlayerStateBase>(NewState);
	return PlayerState && Super::CanTransitionTo(NewState)
		&& CanTransitionToStatus(PlayerState->GetPlayerStatus());
}

bool UPlayerStateBase::RequestPlayerState(EPlayerStatus NewStatus)
{
	UPlayerStateMachine* Machine = Cast<UPlayerStateMachine>(m_pStateMachine);
	return Machine && Machine->ChangePlayerState(NewStatus);
}

void UPlayerStateBase::OnEnter_Implementation() {}
void UPlayerStateBase::OnUpdate_Implementation(float DeltaTime) {}
void UPlayerStateBase::OnExit_Implementation() {}
bool UPlayerStateBase::CanEnterState_Implementation() const { return true; }
bool UPlayerStateBase::CanExitState_Implementation() const { return true; }
bool UPlayerStateBase::CanTransitionToStatus_Implementation(EPlayerStatus NewStatus) const { return true; }

APlayerCharacter* UPlayerStateBase::GetPlayer() const
{
	return IsValid(Player) ? Player.Get() : nullptr;
}
