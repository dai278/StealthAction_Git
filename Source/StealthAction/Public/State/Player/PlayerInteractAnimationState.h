#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerInteractAnimationState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerInteractAnimationState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerInteractAnimationState();
	bool BeginInteraction(class AInteract* Target);
	void CompleteInteraction();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "0.0"))
	float InteractionDuration = 2.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bWaitForAnimationNotify = false;
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	TWeakObjectPtr<class AInteract> InteractionTarget;
	bool bInteractionExecuted = false;
};
