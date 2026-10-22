#pragma once

#include "CoreMinimal.h"
#include "State/StateMachineComponent.h"
#include "PlayerStateMachine.generated.h"

class UPlayerStateBase;

UCLASS(BlueprintType, Blueprintable)
class STEALTHACTION_API UPlayerStateMachine : public UStateMachine
{
	GENERATED_BODY()

public:
	//コンストラクタ
	UPlayerStateMachine();
	//初期化
	virtual void Initialize(AActor* Owner) override;
	//終了処理
	virtual void Stop() override;
	//更新処理
	virtual void Update(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	UPlayerStateBase* GetPlayerState(TSubclassOf<UPlayerStateBase> StateClass) const;

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool CanControl() const;

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetStaminaUsed() const { return StaminaUsed; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaximumStamina() const { return FMath::Max(0.f, MaximumStamina); }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool IsStaminaDepleted() const { return MaximumStamina <= 0.f || StaminaUsed >= MaximumStamina; }

	bool IsInvincible() const;
	bool IsAttacking() const;
	bool IsJumpRequested() const { return bJumpRequested; }
	//移動入力
	void MoveInput();
	void SetDashRequested(bool bRequested);
	void ToggleCrouch();
	void TryJump();
	void TryAttack();
	void ToggleShadow();
	void TryInteract();
	//着地通知
	void NotifyLanded();
	void NotifyAttackFinished();
	void NotifyInteractionFinished();
	//被ダメージ処理
	void ReceiveDamage(int32 Amount, const FVector& Knockback);
	//入力に合わせて移動ステートを変更
	void RefreshLocomotionState();
	bool ReturnToLocomotion(bool bAfterLanding = false);
	void CancelDash() { bDashRequested = false; }
	void CancelLocomotionRequests() { bDashRequested = false; bCrouchRequested = false; }

	UFUNCTION(BlueprintCallable, Category = "Player State Machine")
	bool ChangePlayerState(TSubclassOf<UPlayerStateBase> NewStateClass);

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool IsLocomotionState() const;

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool IsConfigured() const { return bConfigured; }

protected:
	//開始ステート
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine")
	TSubclassOf<UPlayerStateBase> InitialStateClass;

	//スタミナ上限
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
	float MaximumStamina = 5.f;

	//スタミナ回復速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
	float StaminaRecoveryRate = 1.5f;

private:
	TSubclassOf<UPlayerStateBase> SelectLocomotionState(bool bAfterLanding = false) const;
	float StaminaUsed = 0.f;
	bool bDashRequested = false;
	bool bCrouchRequested = false;
	bool bJumpRequested = false;

	bool bConfigured = false;
	bool bTransitionInProgress = false;
};
