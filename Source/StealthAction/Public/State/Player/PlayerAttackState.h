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
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};
