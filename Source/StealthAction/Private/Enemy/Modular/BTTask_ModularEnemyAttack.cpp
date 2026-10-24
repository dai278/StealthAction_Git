//----------------------------------------------------------
//概要：敵の攻撃タスク
//----------------------------------------------------------

#include "Enemy/Modular/BTTask_ModularEnemyAttack.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UBTTask_ModularEnemyAttack::UBTTask_ModularEnemyAttack()
{
	NodeName = TEXT("Modular Enemy Attack");

	//複数の敵で攻撃中の情報を共有しない
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

//-----------------------------------------------------
//タスク開始
//-----------------------------------------------------
EBTNodeResult::Type UBTTask_ModularEnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	AModularEnemyCharacter* Enemy = AI ? Cast<AModularEnemyCharacter>(AI->GetPawn()) : nullptr;
	Attacker = Enemy;
	return Enemy && Enemy->BeginAttack() ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

//-----------------------------------------------------
//タスク更新
//-----------------------------------------------------
void UBTTask_ModularEnemyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!Attacker.IsValid() || !Attacker->IsAlive())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!Attacker->IsAttackActive())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

//-----------------------------------------------------
//タスク中断
//-----------------------------------------------------
EBTNodeResult::Type UBTTask_ModularEnemyAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (Attacker.IsValid())
	{
		Attacker->CancelAttack();
	}
	Attacker.Reset();
	return EBTNodeResult::Aborted;
}
