#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ModularEnemyPatrolPoint.generated.h"

//次の巡回地点をBlackboardへ入れる
UCLASS()
class STEALTHACTION_API UBTTask_ModularEnemyPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_ModularEnemyPatrolPoint();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
