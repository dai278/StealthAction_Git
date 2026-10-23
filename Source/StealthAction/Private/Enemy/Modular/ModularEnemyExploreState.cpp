#include "Enemy/Modular/ModularEnemyExploreState.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "Enemy/Modular/ModularEnemyStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UModularEnemyExploreState::OnEnter_Implementation()
{
	if (AModularEnemyCharacter* Enemy = GetEnemy())
	{
		Enemy->GetCharacterMovement()->MaxWalkSpeed = FMath::Max(0.f, Enemy->ExploreSpeed);
	}
	if (AModularEnemyAIController* AI = GetEnemyController())
	{
		if (!IsValid(BehaviorTree)) { UE_LOG(LogTemp, Warning, TEXT("%s: Set Behavior Tree in the Explore state BP."), *GetName()); }
		AI->RequestBehaviorTree(BehaviorTree.Get());
	}
}
