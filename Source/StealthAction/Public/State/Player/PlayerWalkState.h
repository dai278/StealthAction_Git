//担当日髙
//----------------------------------------------------------
//概要：歩きステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerWalkState.generated.h"

UCLASS(Blueprintable)

class STEALTHACTION_API UPlayerWalkState : public UPlayerLocomotionState
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	UPlayerWalkState();

  protected:
};
