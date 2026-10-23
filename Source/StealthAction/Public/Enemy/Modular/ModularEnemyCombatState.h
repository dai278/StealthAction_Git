#pragma once

#include "CoreMinimal.h"
#include "Enemy/Modular/ModularEnemyState.h"
#include "ModularEnemyCombatState.generated.h"

//戦闘ステート
UCLASS(Blueprintable)
class STEALTHACTION_API UModularEnemyCombatState : public UModularEnemyState
{
	GENERATED_BODY()
protected:
	//このステートで動かすBT
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy State")
	TObjectPtr<class UBehaviorTree> BehaviorTree = nullptr;
	virtual void OnEnter_Implementation() override;
};
