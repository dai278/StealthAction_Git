#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerShadowState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerShadowState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerShadowState();
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};
