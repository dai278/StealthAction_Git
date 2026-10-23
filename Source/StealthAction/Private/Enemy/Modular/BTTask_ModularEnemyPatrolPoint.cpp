#include "Enemy/Modular/BTTask_ModularEnemyPatrolPoint.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_ModularEnemyPatrolPoint::UBTTask_ModularEnemyPatrolPoint()
{
	NodeName = TEXT("Modular Enemy Patrol Point");
}

EBTNodeResult::Type UBTTask_ModularEnemyPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AModularEnemyAIController* AI = Cast<AModularEnemyAIController>(OwnerComp.GetAIOwner());
	return AI && AI->SetNextPatrolLocation() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
