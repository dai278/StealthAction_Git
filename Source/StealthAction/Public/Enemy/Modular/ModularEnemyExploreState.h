//----------------------------------------------------------
//概要：探索ステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Modular/ModularEnemyState.h"
#include "ModularEnemyExploreState.generated.h"

//探索ステート
UCLASS(Blueprintable)

class STEALTHACTION_API UModularEnemyExploreState : public UModularEnemyState
{
	GENERATED_BODY()

  protected:

	//このステートで動かすBT
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy State")
	TObjectPtr<class UBehaviorTree> BehaviorTree = nullptr;

	//開始処理
	virtual void OnEnter_Implementation() override;
};
