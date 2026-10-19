#pragma once

#include "CoreMinimal.h"
#include "State/StateMachineComponent.h"
#include "State/Player/PlayerStatus.h"
#include "PlayerStateMachine.generated.h"

class UPlayerStateBase;

UCLASS(BlueprintType, Blueprintable)
class STEALTHACTION_API UPlayerStateMachine : public UStateMachine
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* Owner) override;
	virtual void Stop() override;
	virtual void Update(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	UPlayerStateBase* GetPlayerState(EPlayerStatus Status) const;

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
	void MoveInput();
	void SetDashRequested(bool bRequested);
	void ToggleCrouch();
	void TryJump();
	void TryAttack();
	void ToggleShadow();
	void TryInteract();
	void NotifyLanded();
	void NotifyAttackFinished();
	void NotifyInteractionFinished();
	void ReceiveDamage(int32 Amount, const FVector& Knockback);
	void RefreshLocomotionState();
	bool ReturnToLocomotion(bool bAfterLanding = false);
	void CancelDash() { bDashRequested = false; }
	void CancelLocomotionRequests() { bDashRequested = false; bCrouchRequested = false; }


	UFUNCTION(BlueprintCallable, Category = "Player State Machine")
	bool ChangePlayerState(EPlayerStatus NewStatus);

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	EPlayerStatus GetPlayerStatus() const;

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool IsLocomotionState() const;

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool IsConfigured() const { return bConfigured; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine")
	EPlayerStatus InitialStatus = EPlayerStatus::Idle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
	float MaximumStamina = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
	float StaminaRecoveryRate = 1.5f;

private:
	EPlayerStatus SelectLocomotionStatus(bool bAfterLanding = false) const;
	float StaminaUsed = 0.f;
	bool bDashRequested = false;
	bool bCrouchRequested = false;
	bool bJumpRequested = false;

	UPROPERTY(Transient)
	TMap<EPlayerStatus, TObjectPtr<UPlayerStateBase>> PlayerStates;

	bool bConfigured = false;
	bool bTransitionInProgress = false;
};
