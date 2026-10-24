//----------------------------------------------------------
//概要：死亡ステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Modular/ModularEnemyState.h"
#include "ModularEnemyDeadState.generated.h"

//死亡ステート
UCLASS(Blueprintable)

class STEALTHACTION_API UModularEnemyDeadState : public UModularEnemyState
{
	GENERATED_BODY()

  protected:

	//開始処理
	virtual void OnEnter_Implementation() override;
};
