#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerAttackState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerAttackState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerAttackState();
	virtual void BackgroundUpdate(float DeltaTime) override;
	void NotifyAttackFinished();
	bool IsAttackActive() const { return bAttackActive; }
	class AEnemyBase* FindTarget() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackRange = 150.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackRadius = 70.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0"))
	int32 SneakKillDamage = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.01"))
	float AttackDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackCooldown = 0.3f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float MinimumFacingDot = 0.4f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackGravityScale = 0.1f;
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	float CooldownRemaining = 0.f;
	float SavedGravityScale = 1.f;
	bool bAttackActive = false;
};
