#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerJumpState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerJumpState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerJumpState();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "0.0"))
	float JumpImpulse = 1500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "1.0"))
	float GravityCurveBase = 10000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "0.0"))
	float MaximumFallSpeed = 1800.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "0"))
	int32 JumpNoiseLevel = 3;
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	bool bCustomJump = false;
	float SavedGravityScale = 1.f;
};
