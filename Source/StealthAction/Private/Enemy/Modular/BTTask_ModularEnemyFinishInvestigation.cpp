#include "Enemy/Modular/BTTask_ModularEnemyFinishInvestigation.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_ModularEnemyFinishInvestigation::UBTTask_ModularEnemyFinishInvestigation()
{
	NodeName = TEXT("Modular Enemy Finish Investigation");
}

EBTNodeResult::Type UBTTask_ModularEnemyFinishInvestigation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AModularEnemyAIController* AI = Cast<AModularEnemyAIController>(OwnerComp.GetAIOwner());
	if (!AI) { return EBTNodeResult::Failed; }
	AI->FinishInvestigation();
	return EBTNodeResult::Succeeded;
}
