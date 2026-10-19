#include "State/Player/PlayerAttackState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Enemy/EnemyBase.h"
#include "EnemyManager/EnemyManager.h"
#include "Sword/SwordAttackComponent.h"
#include "Engine/World.h"

UPlayerAttackState::UPlayerAttackState()
{
	PlayerStatus = EPlayerStatus::Attack;
	MoveSpeed = 0.f;
	bAllowManualControl = false;
}

AEnemyBase* UPlayerAttackState::FindTarget() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UWorld* World = GetWorld();
	if (!PlayerCharacter || !World) { return nullptr; }
	UEnemyManager* Enemies = World->GetSubsystem<UEnemyManager>();
	return IsValid(Enemies) ? Enemies->GetNearestEnemy(PlayerCharacter->GetActorLocation(), FMath::Max(0.f, AttackRange)) : nullptr;
}

bool UPlayerAttackState::CanEnterState_Implementation() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	AEnemyBase* Target = FindTarget();
	if (!PlayerCharacter || !IsValid(Target) || Target->IsPlayerFound() || CooldownRemaining > 0.f
		|| !IsValid(PlayerCharacter->m_sword) || !IsValid(PlayerCharacter->GetMesh())
		|| !IsValid(PlayerCharacter->m_pSpringArm)) { return false; }
	const UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement();
	if (!IsValid(Movement) || !Movement->IsMovingOnGround()) { return false; }
	const FVector Direction = (Target->GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
	return FVector::DotProduct(Direction, PlayerCharacter->GetMesh()->GetRightVector()) > MinimumFacingDot;
}

void UPlayerAttackState::OnEnter_Implementation()
{
	bAttackActive = false;
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter || !IsValid(PlayerCharacter->m_sword)) { return; }
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->CancelDash(); }
	bAttackActive = true;
	if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
	{
		SavedGravityScale = Movement->GravityScale;
		Movement->GravityScale = FMath::Max(0.f, AttackGravityScale);
	}
	USwordAttackComponent* Sword = PlayerCharacter->m_sword;
	Sword->SetSwordAttackScale(FMath::Max(0.f, AttackRadius));
	Sword->Swinging(SneakKillDamage, PlayerCharacter->GetActorLocation(), FMath::Max(0.f, AttackRadius), FMath::Max(0.01f, AttackDuration), true);
	if (IsValid(PlayerCharacter->m_pSpringArm))
	{
		PlayerCharacter->m_pSpringArm->SetWorldRotation(PlayerCharacter->GetControlRotation());
		PlayerCharacter->m_pSpringArm->bUsePawnControlRotation = false;
	}
	PlayerCharacter->OnSneakKillStarted.Broadcast();
	if (IsValid(PlayerCharacter)) { PlayerCharacter->OnSneakKillStarted_BP(); }
}

void UPlayerAttackState::OnUpdate_Implementation(float DeltaTime)
{
	if (!bAttackActive || m_stateTimer >= FMath::Max(0.01f, AttackDuration)) { NotifyAttackFinished(); }
}

void UPlayerAttackState::NotifyAttackFinished()
{
	bAttackActive = false;
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->ReturnToLocomotion(); }
}

void UPlayerAttackState::OnExit_Implementation()
{
	bAttackActive = false;
	CooldownRemaining = FMath::Max(0.f, AttackCooldown);
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (IsValid(PlayerCharacter->m_sword)) { PlayerCharacter->m_sword->SwingEnd(); }
		if (IsValid(PlayerCharacter->m_pSpringArm)) { PlayerCharacter->m_pSpringArm->bUsePawnControlRotation = true; }
		if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement()) { Movement->GravityScale = SavedGravityScale; }
		PlayerCharacter->m_cameraStatus = ECameraStatus::ThirdPerson;
		PlayerCharacter->m_bCameraSwitching = true;
	}
}

void UPlayerAttackState::BackgroundUpdate(float DeltaTime)
{
	CooldownRemaining = FMath::Max(0.f, CooldownRemaining - DeltaTime);
}
