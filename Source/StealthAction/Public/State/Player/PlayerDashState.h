#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerDashState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerDashState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	//コンストラクタ
	UPlayerDashState();

protected:
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnExit_Implementation() override;

};
