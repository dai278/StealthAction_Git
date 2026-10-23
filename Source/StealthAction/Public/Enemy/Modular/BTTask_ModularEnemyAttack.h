#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ModularEnemyAttack.generated.h"

class AModularEnemyCharacter;

//攻撃終了を待つ。中断時は攻撃も止める
UCLASS()
class STEALTHACTION_API UBTTask_ModularEnemyAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_ModularEnemyAttack();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	TWeakObjectPtr<AModularEnemyCharacter> Attacker;
};
