#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "Enemy/Modular/ModularEnemyStateMachine.h"
#include "Enemy/Modular/ModularEnemyCombatState.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
AModularEnemyCharacter::AModularEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AModularEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	Tags.AddUnique(TEXT("Enemy"));
}

//-----------------------------------------------------
//初期化
//-----------------------------------------------------
void AModularEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = FMath::Max(1, MaximumHealth);
	HomeLocation = GetActorLocation();
	if (AModularEnemyAIController* AI = GetEnemyController()) { AI->InitializeEnemy(this); }
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void AModularEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaTime = FMath::Max(0.f, DeltaTime);
	CooldownRemaining = FMath::Max(0.f, CooldownRemaining - DeltaTime);
	if (!bAttackActive) { return; }
	AModularEnemyAIController* AI = GetEnemyController();
	AActor* Target = AttackTarget.Get();
	if (!IsAlive() || !AI || !IsValid(Target)) { CancelAttack(); return; }
	AttackElapsed += DeltaTime;
	if (!bDamageApplied && AttackElapsed >= FMath::Max(0.f, AttackWindup))
	{
		//構えた時の対象にだけ1回判定する
		bDamageApplied = true;
		if (AI->GetVisibleTarget() == Target && AI->LineOfSightTo(Target)
			&& FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) <= FMath::Square(FMath::Max(1.f, AttackRange)))
		{
			if (IDamageable* Damageable = Cast<IDamageable>(Target))
			{
				Damageable->OnDamage(AttackDamage, FVector::ZeroVector, false);
			}
		}
	}
	if (AttackElapsed >= FMath::Max(FMath::Max(0.f, AttackWindup), FMath::Max(0.01f, AttackDuration))) { CancelAttack(); }
}

//-----------------------------------------------------
//攻撃開始
//-----------------------------------------------------
bool AModularEnemyCharacter::BeginAttack()
{
	AModularEnemyAIController* AI = GetEnemyController();
	UModularEnemyStateMachine* Machine = AI ? AI->GetEnemyStateMachine() : nullptr;
	AActor* Target = AI ? AI->GetVisibleTarget() : nullptr;
	if (!IsAlive() || bAttackActive || CooldownRemaining > 0.f || !Machine
		|| !Machine->IsCurrentState<UModularEnemyCombatState>() || !IsValid(Target) || !AI->LineOfSightTo(Target)
		|| FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) > FMath::Square(FMath::Max(1.f, AttackRange))) { return false; }
	AI->StopMovement();
	AttackTarget = Target;
	AttackElapsed = 0.f;
	bDamageApplied = false;
	bAttackActive = true;
	if (IsValid(AttackMontage)) { PlayAnimMontage(AttackMontage.Get()); }
	return true;
}

//-----------------------------------------------------
//攻撃中断
//-----------------------------------------------------
void AModularEnemyCharacter::CancelAttack()
{
	if (!bAttackActive) { return; }
	bAttackActive = false;
	AttackTarget.Reset();
	CooldownRemaining = FMath::Max(0.f, AttackCooldown);
	if (IsValid(AttackMontage)) { StopAnimMontage(AttackMontage.Get()); }
}

//-----------------------------------------------------
//被ダメージ処理
//-----------------------------------------------------
void AModularEnemyCharacter::OnDamage(int32 Damage, FVector Knockback, bool bSneakKill)
{
	if (!IsAlive() || Damage <= 0) { return; }
	Health = Damage >= Health ? 0 : Health - Damage;
	if (IsAlive() && !Knockback.IsNearlyZero()) { LaunchCharacter(Knockback, true, true); }
	if (AModularEnemyAIController* AI = GetEnemyController())
	{
		if (UModularEnemyStateMachine* Machine = AI->GetEnemyStateMachine()) { Machine->NotifyDamage(); }
	}
	//設定失敗時も死亡後の攻撃は残さない
	if (!IsAlive()) { ApplyDeath(); }
}

//-----------------------------------------------------
//Apply Damageからの被ダメージ
//-----------------------------------------------------
float AModularEnemyCharacter::TakeDamage(float Amount, const FDamageEvent& Event, AController* InstigatorController, AActor* Causer)
{
	const float Accepted = Super::TakeDamage(Amount, Event, InstigatorController, Causer);
	if (!IsAlive() || Accepted <= 0.f) { return 0.f; }
	OnDamage(FMath::CeilToInt(Accepted), FVector::ZeroVector, false);
	return Accepted;
}

//-----------------------------------------------------
//巡回先取得
//-----------------------------------------------------
FVector AModularEnemyCharacter::GetNextPatrolLocation()
{
	for (int32 Attempt = 0; Attempt < PatrolPoints.Num(); ++Attempt)
	{
		PatrolIndex %= PatrolPoints.Num();
		AActor* Point = PatrolPoints[PatrolIndex++].Get();
		if (IsValid(Point)) { return Point->GetActorLocation(); }
	}
	return HomeLocation;
}

//-----------------------------------------------------
//死亡処理
//-----------------------------------------------------
void AModularEnemyCharacter::ApplyDeath()
{
	if (bDeathApplied) { return; }
	bDeathApplied = true;
	CancelAttack();
	if (AModularEnemyAIController* AI = GetEnemyController()) { AI->StopEnemyLogic(); }
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnDeath();
}

AModularEnemyAIController* AModularEnemyCharacter::GetEnemyController() const
{
	return Cast<AModularEnemyAIController>(GetController());
}

void AModularEnemyCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	CancelAttack();
	if (AModularEnemyAIController* AI = GetEnemyController()) { AI->StopEnemyLogic(true); }
	Super::EndPlay(Reason);
}
