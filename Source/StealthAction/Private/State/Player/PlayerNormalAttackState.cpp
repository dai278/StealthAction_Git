//担当日髙
//----------------------------------------------------------
//概要：通常攻撃ステート
//----------------------------------------------------------

#include "State/Player/PlayerNormalAttackState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Enemy/EnemyBase.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Damage/IDamage.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerNormalAttackState::UPlayerNormalAttackState()
{
	PlayerStatus = EPlayerStatus::NormalAttack;
	MoveSpeed = 0.f;
	bAllowManualControl = false;
}

//-----------------------------------------------------
//開始条件(BPの条件とは別に空中や連続入力を防ぐ)
//-----------------------------------------------------
bool UPlayerNormalAttackState::CanEnter() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	const UCharacterMovementComponent* Movement = PlayerCharacter ? PlayerCharacter->GetCharacterMovement() : nullptr;
	return PlayerCharacter && PlayerCharacter->IsAlive() && IsValid(PlayerCharacter->GetMesh()) && IsValid(Movement) &&
		   Movement->IsMovingOnGround() && !bAttackActive && CooldownRemaining <= 0.f && Super::CanEnter();
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerNormalAttackState::OnEnter_Implementation()
{
	bAttackActive = true;
	bAttackCancelled = false;
	bMontageEnded = false;
	PreviousAttackTime = 0.f;
	ActiveDuration = FMath::Max(0.01f, AttackDuration);
	HitActors.Reset();
	PlayingAnimInstance.Reset();
	APlayerCharacter* PlayerCharacter = GetPlayer();
	if (!PlayerCharacter)
	{
		bAttackCancelled = true;
		return;
	}
	if (UPlayerStateMachine* Machine = GetPlayerMachine())
	{
		Machine->CancelLocomotionRequests();
	}
	if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}
	if (!IsValid(AttackMontage))
	{
		return;
	}
	USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh();
	UAnimInstance* AnimInstance = IsValid(Mesh) ? Mesh->GetAnimInstance() : nullptr;
	const float Duration = IsValid(AnimInstance) ? AnimInstance->Montage_Play(AttackMontage.Get(), FMath::Max(0.01f, MontagePlayRate),
																			  EMontagePlayReturnType::Duration)
												 : 0.f;
	if (Duration <= 0.f)
	{
		bAttackCancelled = true;
		UE_LOG(LogTemp, Warning, TEXT("%s: Normal attack montage could not be played."), *GetName());
		return;
	}
	ActiveDuration = Duration;
	PlayingAnimInstance = AnimInstance;
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UPlayerNormalAttackState::OnAttackMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage.Get());
	FOnMontageBlendingOutStarted BlendDelegate;
	BlendDelegate.BindUObject(this, &UPlayerNormalAttackState::OnAttackMontageBlendingOut);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendDelegate, AttackMontage.Get());
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerNormalAttackState::OnUpdate_Implementation(float DeltaTime)
{
	if (IsValid(AttackMontage) && !PlayingAnimInstance.IsValid())
	{
		bAttackCancelled = true;
	}
	if (bAttackActive && !bAttackCancelled)
	{
		const float Start = FMath::Max(0.f, DamageStartTime);
		const float End = Start + FMath::Max(0.f, DamageWindowDuration);

		//フレームをまたいで判定時間を通り過ぎても1回は調べる
		if (Start <= ActiveDuration && m_stateTimer >= Start && PreviousAttackTime <= End)
		{
			ApplyAttackHits();
		}
	}
	PreviousAttackTime = m_stateTimer;
	if (!bAttackActive || bAttackCancelled || bMontageEnded || m_stateTimer >= ActiveDuration)
	{
		bAttackActive = false;
		if (UPlayerStateMachine* Machine = GetPlayerMachine())
		{
			Machine->ReturnToLocomotion();
		}
	}
}

//-----------------------------------------------------
//前方の敵に通常ダメージを与える
//-----------------------------------------------------
void UPlayerNormalAttackState::ApplyAttackHits()
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UWorld* World = GetWorld();
	if (!PlayerCharacter || !World)
	{
		return;
	}
	const FVector Start = PlayerCharacter->GetActorLocation() + PlayerCharacter->GetActorTransform().TransformVectorNoScale(AttackOffset);
	const FVector End = Start + PlayerCharacter->GetActorForwardVector() * FMath::Max(0.f, AttackDistance);
	FCollisionObjectQueryParams Enemies;
	Enemies.AddObjectTypesToQuery(ECC_GameTraceChannel2); //Enemy
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerCharacter);
	TArray<FHitResult> Hits;
	World->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, Enemies, FCollisionShape::MakeSphere(FMath::Max(1.f, AttackRadius)),
								  Params);
	for (const FHitResult& Hit : Hits)
	{
		AActor* Target = Hit.GetActor();
		const TWeakObjectPtr<AActor> Key(Target);
		if (!IsValid(Target) || Target == PlayerCharacter || HitActors.Contains(Key))
		{
			continue;
		}
		if (const AEnemyBase* Enemy = Cast<AEnemyBase>(Target))
		{
			if (Enemy->IsDead())
			{
				continue;
			}
		}
		if (const AModularEnemyCharacter* Enemy = Cast<AModularEnemyCharacter>(Target))
		{
			if (!Enemy->IsAlive())
			{
				continue;
			}
		}
		IDamageable* Damageable = Cast<IDamageable>(Target);
		if (!Damageable)
		{
			continue;
		}

		//範囲内でも壁や扉の向こうには当てない
		FCollisionObjectQueryParams Obstacles;
		Obstacles.AddObjectTypesToQuery(ECC_WorldStatic);
		Obstacles.AddObjectTypesToQuery(ECC_WorldDynamic);
		FCollisionQueryParams VisibilityParams;
		VisibilityParams.AddIgnoredActor(PlayerCharacter);
		VisibilityParams.AddIgnoredActor(Target);
		FHitResult ObstacleHit;
		if (World->LineTraceSingleByObjectType(ObstacleHit, PlayerCharacter->GetActorLocation(), Target->GetActorLocation(), Obstacles,
											   VisibilityParams))
		{
			continue;
		}
		HitActors.Add(Key);
		FVector Knockback = Target->GetActorLocation() - PlayerCharacter->GetActorLocation();
		Knockback.Z = 0.f;
		Damageable->OnDamage(AttackDamage, Knockback.GetSafeNormal() * FMath::Max(0.f, KnockbackForce), false);

		//ダメージ通知中にプレイヤーが中断された場合は残りを実行しない
		if (!bAttackActive || bAttackCancelled)
		{
			break;
		}
	}
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerNormalAttackState::OnExit_Implementation()
{
	bAttackActive = false;
	bAttackCancelled = true;
	CooldownRemaining = FMath::Max(0.f, AttackCooldown);
	if (UAnimInstance* AnimInstance = PlayingAnimInstance.Get())
	{
		FOnMontageEnded EmptyEnd;
		FOnMontageBlendingOutStarted EmptyBlend;
		AnimInstance->Montage_SetEndDelegate(EmptyEnd, AttackMontage.Get());
		AnimInstance->Montage_SetBlendingOutDelegate(EmptyBlend, AttackMontage.Get());
		if (AnimInstance->Montage_IsActive(AttackMontage.Get()))
		{
			AnimInstance->Montage_Stop(FMath::Max(0.f, MontageBlendOutTime), AttackMontage.Get());
		}
	}
	PlayingAnimInstance.Reset();
	HitActors.Reset();
}

//-----------------------------------------------------
//攻撃終了後の待ち時間
//-----------------------------------------------------
void UPlayerNormalAttackState::BackgroundUpdate(float DeltaTime)
{
	CooldownRemaining = FMath::Max(0.f, CooldownRemaining - FMath::Max(0.f, DeltaTime));
}

//-----------------------------------------------------
//攻撃モンタージュ終了通知
//-----------------------------------------------------
void UPlayerNormalAttackState::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bAttackActive || Montage != AttackMontage.Get())
	{
		return;
	}
	bMontageEnded = true;
	bAttackCancelled = bAttackCancelled || bInterrupted;
}

//-----------------------------------------------------
//攻撃モンタージュの中断確認
//-----------------------------------------------------
void UPlayerNormalAttackState::OnAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bAttackActive && Montage == AttackMontage.Get() && bInterrupted)
	{
		bAttackCancelled = true;
	}
}
