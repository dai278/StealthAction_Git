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
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};
