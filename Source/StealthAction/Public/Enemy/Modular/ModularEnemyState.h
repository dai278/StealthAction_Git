#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "ModularEnemyState.generated.h"

class AModularEnemyCharacter;
class AModularEnemyAIController;
class UModularEnemyStateMachine;

//敵ステートの共通処理
UCLASS(Abstract, Blueprintable)
class STEALTHACTION_API UModularEnemyState : public UStateBase
{
	GENERATED_BODY()
public:
	virtual UWorld* GetWorld() const override;
	UFUNCTION(BlueprintPure, Category = "Enemy State")
	AModularEnemyCharacter* GetEnemy() const;
protected:
	AModularEnemyAIController* GetEnemyController() const;
	UModularEnemyStateMachine* GetEnemyMachine() const;
	//BPで親を呼ばなくても前のBTを停止する
	virtual void Exit() override;
};
