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
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};
