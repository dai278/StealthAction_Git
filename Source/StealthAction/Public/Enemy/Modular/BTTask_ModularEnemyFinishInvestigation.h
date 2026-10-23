#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ModularEnemyFinishInvestigation.generated.h"

//調査済みの位置を繰り返し調べないようにする
UCLASS()
class STEALTHACTION_API UBTTask_ModularEnemyFinishInvestigation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_ModularEnemyFinishInvestigation();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
