#include "Enemy/Modular/ModularEnemyDeadState.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "Enemy/Modular/ModularEnemyStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UModularEnemyDeadState::OnEnter_Implementation()
{
	if (AModularEnemyCharacter* Enemy = GetEnemy()) { Enemy->ApplyDeath(); }
}
