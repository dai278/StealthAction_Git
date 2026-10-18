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
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};
