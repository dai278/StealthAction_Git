//----------------------------------------------------------
//概要：調査完了タスク
//----------------------------------------------------------

#include "Enemy/Modular/BTTask_ModularEnemyFinishInvestigation.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UBTTask_ModularEnemyFinishInvestigation::UBTTask_ModularEnemyFinishInvestigation()
{
	NodeName = TEXT("Modular Enemy Finish Investigation");
}

//-----------------------------------------------------
//タスク開始
//-----------------------------------------------------
EBTNodeResult::Type UBTTask_ModularEnemyFinishInvestigation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AModularEnemyAIController* AI = Cast<AModularEnemyAIController>(OwnerComp.GetAIOwner());
	if (!AI)
	{
		return EBTNodeResult::Failed;
	}
	AI->FinishInvestigation();
	return EBTNodeResult::Succeeded;
}
