#include "State/Player/PlayerDamageState.h"
#include "State/Player/PlayerDeadState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerDamageState::UPlayerDamageState()
{
	MoveSpeed = 0.f;
	bAllowManualControl = false;
}

//-----------------------------------------------------
//被ダメージ処理
//-----------------------------------------------------
void UPlayerDamageState::ReceiveDamage(int32 Amount, const FVector& Knockback)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UPlayerStateMachine* Machine = GetPlayerMachine();
	if (!PlayerCharacter || !Machine || !Machine->IsConfigured() || IsInvincible()
		|| !PlayerCharacter->IsAlive() || Amount <= 0 || Machine->IsCurrentState<UPlayerDeadState>()) { return; }
	//ステート変更できなかった場合にHPを戻すため保存
	const int32 PreviousHP = PlayerCharacter->m_playerInfo.hp;
	PlayerCharacter->m_playerInfo.hp = Amount >= PreviousHP ? 0 : PreviousHP - Amount;
	PendingKnockback = Knockback * FMath::Max(0.f, KnockbackMultiplier);
	const TSubclassOf<UPlayerStateBase> Next = PlayerCharacter->m_playerInfo.hp <= 0 ? UPlayerDeadState::StaticClass() : UPlayerDamageState::StaticClass();
	if (!Machine->ChangePlayerState(Next)) { PlayerCharacter->m_playerInfo.hp = PreviousHP; }
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerDamageState::OnEnter_Implementation()
{
	InvincibilityRemaining = FMath::Max(InvincibilityDuration, RecoveryDuration);
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->CancelDash(); }
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent())
		{
			Capsule->SetCollisionResponseToChannel(ECC_GameTraceChannel2, InvincibilityRemaining > 0.f ? ECR_Ignore : ECR_Block);
		}
		if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
		{
			Movement->Velocity.Z = FMath::Min(0.0, static_cast<double>(Movement->Velocity.Z));
		}
		PlayerCharacter->LaunchCharacter(PendingKnockback, true, false);
		PlayerCharacter->m_cameraStatus = ECameraStatus::ThirdPerson;
		PlayerCharacter->m_bCameraSwitching = true;
	}
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerDamageState::OnUpdate_Implementation(float DeltaTime)
{
	if (m_stateTimer >= FMath::Max(0.f, RecoveryDuration))
	{
		if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->ReturnToLocomotion(); }
	}
}

//-----------------------------------------------------
//他のステート中も更新する処理
//-----------------------------------------------------
void UPlayerDamageState::BackgroundUpdate(float DeltaTime)
{
	if (InvincibilityRemaining <= 0.f) { return; }
	InvincibilityRemaining = FMath::Max(0.f, InvincibilityRemaining - DeltaTime);
	if (InvincibilityRemaining <= 0.f) { ClearProtection(); }
}

//-----------------------------------------------------
//無敵解除
//-----------------------------------------------------
void UPlayerDamageState::ClearProtection()
{
	InvincibilityRemaining = 0.f;
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		if (UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent())
		{
			Capsule->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
		}
	}
}
