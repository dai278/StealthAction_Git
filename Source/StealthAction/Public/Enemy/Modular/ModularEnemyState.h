//担当日髙
//----------------------------------------------------------
//概要：敵ステートの共通処理
//----------------------------------------------------------

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

	//ワールド取得
	virtual UWorld* GetWorld() const override;

	//敵取得
	UFUNCTION(BlueprintPure, Category = "Enemy State")
	AModularEnemyCharacter* GetEnemy() const;

  protected:

	//敵のAIコントローラー取得
	AModularEnemyAIController* GetEnemyController() const;

	//敵のステートマシン取得
	UModularEnemyStateMachine* GetEnemyMachine() const;

	//BPで親を呼ばなくても前のBTを停止する
	virtual void Exit() override;
};
