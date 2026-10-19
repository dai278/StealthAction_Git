#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerInteractState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerInteractState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerInteractState();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "0.0"))
	float AcceptanceRadius = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ApproachInputScale = 0.5f;
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	TWeakObjectPtr<class AInteract> InteractionTarget;
	FVector InteractionPosition = FVector::ZeroVector;
};
