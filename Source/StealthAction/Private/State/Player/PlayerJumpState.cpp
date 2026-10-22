#include "State/Player/PlayerJumpState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Noise/NoiseManager.h"
#include "Engine/World.h"

UPlayerJumpState::UPlayerJumpState() { }

bool UPlayerJumpState::CanEnterState_Implementation() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	const UPlayerStateMachine* Machine = GetPlayerMachine();
	const UCharacterMovementComponent* Movement = PlayerCharacter ? PlayerCharacter->GetCharacterMovement() : nullptr;
	return IsValid(Movement) && Machine && (Machine->IsJumpRequested() ? Movement->IsMovingOnGround() : Movement->IsFalling());
}

void UPlayerJumpState::OnEnter_Implementation()
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UPlayerStateMachine* Machine = GetPlayerMachine();
	UCharacterMovementComponent* Movement = PlayerCharacter ? PlayerCharacter->GetCharacterMovement() : nullptr;
	if (!IsValid(Movement) || !Machine) { return; }
	SavedGravityScale = Movement->GravityScale;
	bCustomJump = Machine->IsJumpRequested();
	if (bCustomJump)
	{
		PlayerCharacter->LaunchCharacter(FVector(0.f, 0.f, FMath::Max(0.f, JumpImpulse)), false, true);
		Movement->GravityScale = 0.f;
		if (UWorld* World = GetWorld())
		{
			if (UNoiseManager* Noise = World->GetSubsystem<UNoiseManager>())
			{
				if (JumpNoiseLevel > 0) { Noise->MakeNoise(JumpNoiseLevel, PlayerCharacter->GetActorLocation()); }
			}
		}
	}
}

void UPlayerJumpState::OnUpdate_Implementation(float DeltaTime)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UCharacterMovementComponent* Movement = PlayerCharacter ? PlayerCharacter->GetCharacterMovement() : nullptr;
	if (!IsValid(Movement)) { return; }
	if (bCustomJump)
	{
		Movement->Velocity.Z = FMath::Max(-static_cast<double>(FMath::Max(0.f, MaximumFallSpeed)),
			static_cast<double>(Movement->Velocity.Z) - static_cast<double>(FMath::Pow(FMath::Max(1.f, GravityCurveBase), m_stateTimer)));
	}
	//着地直後は移動モードが空中のままなのでLanded側で戻す
}

void UPlayerJumpState::OnExit_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement()) { Movement->GravityScale = SavedGravityScale; }
	}
	bCustomJump = false;
}
