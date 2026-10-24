//----------------------------------------------------------
//概要：敵の攻撃タスク
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ModularEnemyAttack.generated.h"

class AModularEnemyCharacter;

//攻撃終了待ち(中断時は攻撃も止める)
UCLASS()

class STEALTHACTION_API UBTTask_ModularEnemyAttack : public UBTTaskNode
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	UBTTask_ModularEnemyAttack();

  protected:

	//タスク開始
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//タスク更新
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//タスク中断
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

  private:

	//攻撃中の敵の保存用
	TWeakObjectPtr<AModularEnemyCharacter> Attacker;
};
