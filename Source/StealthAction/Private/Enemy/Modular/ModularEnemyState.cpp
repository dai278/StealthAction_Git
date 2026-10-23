#include "Enemy/Modular/ModularEnemyState.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "Enemy/Modular/ModularEnemyStateMachine.h"

AModularEnemyCharacter* UModularEnemyState::GetEnemy() const
{
	return IsValid(m_pOwner) ? Cast<AModularEnemyCharacter>(m_pOwner) : nullptr;
}

UWorld* UModularEnemyState::GetWorld() const
{
	const AModularEnemyCharacter* Enemy = GetEnemy();
	return Enemy ? Enemy->GetWorld() : nullptr;
}

AModularEnemyAIController* UModularEnemyState::GetEnemyController() const
{
	const AModularEnemyCharacter* Enemy = GetEnemy();
	return Enemy ? Enemy->GetEnemyController() : nullptr;
}

UModularEnemyStateMachine* UModularEnemyState::GetEnemyMachine() const
{
	return Cast<UModularEnemyStateMachine>(m_pStateMachine.Get());
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UModularEnemyState::Exit()
{
	if (AModularEnemyAIController* AI = GetEnemyController()) { AI->StopEnemyLogic(); }
	if (AModularEnemyCharacter* Enemy = GetEnemy()) { Enemy->CancelAttack(); }
	Super::Exit();
}
