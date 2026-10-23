#include "Enemy/Modular/ModularEnemyDamageState.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "Enemy/Modular/ModularEnemyStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UModularEnemyDamageState::OnEnter_Implementation()
{
	if (AModularEnemyAIController* AI = GetEnemyController()) { AI->StopEnemyLogic(); }
	if (AModularEnemyCharacter* Enemy = GetEnemy()) { Enemy->CancelAttack(); }
}

//-----------------------------------------------------
//ひるみ終了後は今の状況から戻り先を決める
//-----------------------------------------------------
void UModularEnemyDamageState::OnUpdate_Implementation(float DeltaTime)
{
	if (m_stateTimer >= FMath::Max(0.f, RecoveryTime))
	{
		if (UModularEnemyStateMachine* Machine = GetEnemyMachine()) { Machine->ReturnToAction(); }
	}
}
