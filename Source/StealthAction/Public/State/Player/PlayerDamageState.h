#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerDamageState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerDamageState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerDamageState();
	virtual void BackgroundUpdate(float DeltaTime) override;
	void ReceiveDamage(int32 Amount, const FVector& Knockback);
	bool IsInvincible() const { return InvincibilityRemaining > 0.f; }
	void ClearProtection();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float RecoveryDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float InvincibilityDuration = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float KnockbackMultiplier = 1.f;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;

private:
	float InvincibilityRemaining = 0.f;
	FVector PendingKnockback = FVector::ZeroVector;
};
