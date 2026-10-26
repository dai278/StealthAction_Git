//担当日髙
//----------------------------------------------------------
//概要：戦闘ステート
//----------------------------------------------------------

#include "Enemy/Modular/ModularEnemyCombatState.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "Enemy/Modular/ModularEnemyStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UModularEnemyCombatState::OnEnter_Implementation()
{
	if (AModularEnemyCharacter* Enemy = GetEnemy())
	{
		Enemy->GetCharacterMovement()->MaxWalkSpeed = FMath::Max(0.f, Enemy->CombatSpeed);
	}
	if (AModularEnemyAIController* AI = GetEnemyController())
	{
		if (!IsValid(BehaviorTree))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: Set Behavior Tree in the Combat state BP."), *GetName());
		}
		AI->RequestBehaviorTree(BehaviorTree.Get());
	}
}
