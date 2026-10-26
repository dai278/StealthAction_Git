//担当日髙
//----------------------------------------------------------
//概要：敵ステートの共通処理
//----------------------------------------------------------

#include "Enemy/Modular/ModularEnemyState.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyAIController.h"
#include "Enemy/Modular/ModularEnemyStateMachine.h"

//-----------------------------------------------------
//敵取得
//-----------------------------------------------------
AModularEnemyCharacter* UModularEnemyState::GetEnemy() const
{
	return IsValid(m_pOwner) ? Cast<AModularEnemyCharacter>(m_pOwner) : nullptr;
}

//-----------------------------------------------------
//ワールド取得
//-----------------------------------------------------
UWorld* UModularEnemyState::GetWorld() const
{
	const AModularEnemyCharacter* Enemy = GetEnemy();
	return Enemy ? Enemy->GetWorld() : nullptr;
}

//-----------------------------------------------------
//敵のAIコントローラー取得
//-----------------------------------------------------
AModularEnemyAIController* UModularEnemyState::GetEnemyController() const
{
	const AModularEnemyCharacter* Enemy = GetEnemy();
	return Enemy ? Enemy->GetEnemyController() : nullptr;
}

//-----------------------------------------------------
//敵のステートマシン取得
//-----------------------------------------------------
UModularEnemyStateMachine* UModularEnemyState::GetEnemyMachine() const
{
	return Cast<UModularEnemyStateMachine>(m_pStateMachine.Get());
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UModularEnemyState::Exit()
{
	if (AModularEnemyAIController* AI = GetEnemyController())
	{
		AI->StopEnemyLogic();
	}
	if (AModularEnemyCharacter* Enemy = GetEnemy())
	{
		Enemy->CancelAttack();
	}
	Super::Exit();
}
