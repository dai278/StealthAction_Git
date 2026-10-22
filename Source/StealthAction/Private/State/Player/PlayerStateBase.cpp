#include "State/Player/PlayerStateBase.h"
#include "State/Player/PlayerStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Noise/NoiseManager.h"
#include "Engine/World.h"
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
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = FMath::Max(0.f, MoveSpeed);
		}
	}
	if (bOverrideCamera)
	{
		if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->m_bCameraSwitching = true; }
	}
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
	if (bOverrideCamera)
	{
		if (APlayerCharacter* PlayerCharacter = GetPlayer()) { PlayerCharacter->m_bCameraSwitching = true; }
	}
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
		&& CanTransitionToState(PlayerState->GetClass());
}

bool UPlayerStateBase::RequestPlayerState(TSubclassOf<UPlayerStateBase> NewStateClass)
{
	UPlayerStateMachine* Machine = Cast<UPlayerStateMachine>(m_pStateMachine);
	return Machine && Machine->ChangePlayerState(NewStateClass);
}

void UPlayerStateBase::OnEnter_Implementation() {}
void UPlayerStateBase::OnUpdate_Implementation(float DeltaTime) {}
void UPlayerStateBase::OnExit_Implementation() {}
bool UPlayerStateBase::CanEnterState_Implementation() const { return true; }
bool UPlayerStateBase::CanExitState_Implementation() const { return true; }
bool UPlayerStateBase::CanTransitionToState_Implementation(TSubclassOf<UPlayerStateBase> NewStateClass) const { return true; }

APlayerCharacter* UPlayerStateBase::GetPlayer() const
{
	return IsValid(Player) ? Player.Get() : nullptr;
}

UPlayerStateMachine* UPlayerStateBase::GetPlayerMachine() const
{
	return Cast<UPlayerStateMachine>(m_pStateMachine);
}

void UPlayerStateBase::OnMoveInput_Implementation()
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter || !bAllowMovementInput || !PlayerCharacter->CanReceivePlayerInput()) { return; }
	UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement();
	UWorld* World = GetWorld();
	if (!IsValid(Movement) || !World || !Movement->IsMovingOnGround()
		|| PlayerCharacter->m_charaMoveInput.IsNearlyZero(0.1f)) { return; }
	const float Yaw = PlayerCharacter->m_cameraStatus == ECameraStatus::TopDownView
		? PlayerCharacter->GetActorRotation().Yaw : PlayerCharacter->GetControlRotation().Yaw;
	const FRotator Rotation(0.f, Yaw, 0.f);
	FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X) * PlayerCharacter->m_charaMoveInput.Y
		+ FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y) * PlayerCharacter->m_charaMoveInput.X;
	Direction.Z = 0.f;
	if (!Direction.Normalize()) { return; }
	PlayerCharacter->AddMovementInput(Direction, 1.f);
	PlayerCharacter->SetActorRotation(FMath::RInterpTo(PlayerCharacter->GetActorRotation(), Direction.Rotation(),
		World->GetDeltaSeconds(), FMath::Max(0.f, RotationInterpSpeed)));
	if (FootstepNoiseLevel > 0)
	{
		if (UNoiseManager* Noise = World->GetSubsystem<UNoiseManager>())
		{
			Noise->MakeNoise(FootstepNoiseLevel, PlayerCharacter->GetActorLocation());
		}
	}
}

void UPlayerStateBase::OnStaminaDepleted_Implementation()
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine())
	{
		Machine->CancelDash();
		Machine->ReturnToLocomotion();
	}
}
