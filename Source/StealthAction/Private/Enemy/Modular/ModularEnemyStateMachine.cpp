//----------------------------------------------------------
//概要：敵のステート管理
//----------------------------------------------------------

#include "Enemy/Modular/ModularEnemyStateMachine.h"
#include "Enemy/Modular/ModularEnemyState.h"
#include "Enemy/Modular/ModularEnemyExploreState.h"
#include "Enemy/Modular/ModularEnemyCombatState.h"
#include "Enemy/Modular/ModularEnemyDamageState.h"
#include "Enemy/Modular/ModularEnemyDeadState.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyAIController.h"

//-----------------------------------------------------
//初期化
//-----------------------------------------------------
void UModularEnemyStateMachine::Initialize(AActor* Owner)
{
	Super::Initialize(Owner);
	if (!Cast<AModularEnemyCharacter>(Owner))
	{
		return;
	}

	//登録無しの場合はC++ステートを使う。BP登録時は4種類を揃える
	if (GetRegisteredStates().IsEmpty())
	{
		RegisterState<UModularEnemyExploreState>()->Initialize(this, Owner);
		RegisterState<UModularEnemyCombatState>()->Initialize(this, Owner);
		RegisterState<UModularEnemyDamageState>()->Initialize(this, Owner);
		RegisterState<UModularEnemyDeadState>()->Initialize(this, Owner);
	}
	const UClass* Required[] = {UModularEnemyExploreState::StaticClass(), UModularEnemyCombatState::StaticClass(),
								UModularEnemyDamageState::StaticClass(), UModularEnemyDeadState::StaticClass()};
	bConfigured = true;
	for (const UClass* Class : Required)
	{
		int32 Count = 0;
		for (UStateBase* State : GetRegisteredStates())
		{
			if (IsValid(State) && State->IsA(Class))
			{
				++Count;
			}
		}
		if (Count != 1)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Register exactly one subclass of %s."), *GetName(), *Class->GetName());
			bConfigured = false;
		}
	}
	for (UStateBase* State : GetRegisteredStates())
	{
		if (!Cast<UModularEnemyState>(State))
		{
			bConfigured = false;
		}
	}
	if (bConfigured && !ReturnToAction())
	{
		bConfigured = false;
	}
}

//-----------------------------------------------------
//ステート変更
//-----------------------------------------------------
bool UModularEnemyStateMachine::ChangeEnemyState(TSubclassOf<UModularEnemyState> StateClass)
{
	if (!bConfigured || bChangingState || !StateClass || IsCurrentState<UModularEnemyDeadState>())
	{
		return false;
	}
	TGuardValue<bool> Guard(bChangingState, true);
	for (UStateBase* State : GetRegisteredStates())
	{
		if (!IsValid(State) || !State->IsA(StateClass))
		{
			continue;
		}
		if (State == GetCurrentState())
		{
			return false;
		}
		if (State->IsA<UModularEnemyDamageState>() || State->IsA<UModularEnemyDeadState>())
		{
			return ForceChangeState(State);
		}
		return ChangeState(State);
	}
	return false;
}

//-----------------------------------------------------
//探索と戦闘の切り替え
//-----------------------------------------------------
bool UModularEnemyStateMachine::ReturnToAction()
{
	AModularEnemyCharacter* Enemy = Cast<AModularEnemyCharacter>(GetOwner());
	if (!Enemy)
	{
		return false;
	}
	if (!Enemy->IsAlive())
	{
		return ChangeEnemyState(UModularEnemyDeadState::StaticClass());
	}
	const AModularEnemyAIController* AI = Enemy->GetEnemyController();
	return ChangeEnemyState(AI && AI->HasCombatTarget() ? UModularEnemyCombatState::StaticClass()
														: UModularEnemyExploreState::StaticClass());
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UModularEnemyStateMachine::Update(float DeltaTime)
{
	if (!bConfigured)
	{
		return;
	}
	Super::Update(FMath::Max(0.f, DeltaTime));
	if (IsCurrentState<UModularEnemyExploreState>() || IsCurrentState<UModularEnemyCombatState>())
	{
		ReturnToAction();
	}
}

//-----------------------------------------------------
//被ダメージ通知
//-----------------------------------------------------
void UModularEnemyStateMachine::NotifyDamage()
{
	const AModularEnemyCharacter* Enemy = Cast<AModularEnemyCharacter>(GetOwner());
	if (!Enemy || !bConfigured)
	{
		return;
	}
	if (!Enemy->IsAlive())
	{
		ChangeEnemyState(UModularEnemyDeadState::StaticClass());
	}
	else if (IsCurrentState<UModularEnemyDamageState>())
	{
		GetCurrentState()->Reset();
	}
	else
	{
		ChangeEnemyState(UModularEnemyDamageState::StaticClass());
	}
}

//-----------------------------------------------------
//停止処理
//-----------------------------------------------------
void UModularEnemyStateMachine::Stop()
{
	if (bChangingState)
	{
		return;
	}
	TGuardValue<bool> Guard(bChangingState, true);
	Super::Stop();
	bConfigured = false;
}
