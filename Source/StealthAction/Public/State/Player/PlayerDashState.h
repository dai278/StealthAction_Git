//----------------------------------------------------------
//概要：ダッシュステート
//----------------------------------------------------------

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

	//開始条件
	virtual bool CanEnterState_Implementation() const override;

	//開始処理
	virtual void OnEnter_Implementation() override;

	//終了処理
	virtual void OnExit_Implementation() override;
};
