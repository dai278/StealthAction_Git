// Fill out your copyright notice in the Description page of Project Settings.

#include "State/StateMachineComponent.h"
#include "State/StateBase.h"

//==================================================
//コンストラクタ
//==================================================
UStateMachine::UStateMachine()
{
}

//==================================================
//初期化
//==================================================
void UStateMachine::Initialize(AActor* _inOwner)
{
	m_owner = _inOwner;

	//TsubClassのほうに登録されたステートを登録
	for (TSubclassOf<UStateBase> stateClass : m_stateClasses)
	{
		if (!stateClass) { continue; }
		UStateBase* registerState = NewObject<UStateBase>(this, stateClass);
		if(registerState)
		{
			registerState->Initialize(this, _inOwner);
			m_states.Add(registerState);
		}
	}
}

//==================================================
//ステートマシン更新処理
//==================================================
void UStateMachine::Update(float _deltaTime)
{
	if (!IsValid(m_currentState))
	{
		return;
	}

	m_currentState->Update(_deltaTime);
}

//===================================================
//ステート取得(Tag)
//===================================================
UStateBase* UStateMachine::GetState(const FName _tag)
{
	for (UStateBase* state : m_states)
	{
		if (!IsValid(state)) { continue; }
		
		if (state->HasTag(_tag))
		{
			return state;
		}
	}
	return nullptr;
}

//===================================================
//ステート取得(SubClass)
//===================================================
UStateBase* UStateMachine::GetStateBySubClass(TSubclassOf<UStateBase> TargetClass) 
{
	if (!TargetClass){return nullptr;}
	
	for (UStateBase* pState : m_states)
	{
		if (IsValid(pState) && pState->GetClass() == TargetClass.Get())
		{
			return pState; 
		}
	}
	return nullptr;
}

//==================================================
//ステート変更
//==================================================
bool UStateMachine::ChangeState(UStateBase* _newState)
{
	
	 //終了可能か確認 
	if( IsValid(m_currentState))
	{
		if (!m_currentState->CanExit())
		{
			return false;
		}

		
		 //現在ステートが次のステートへ遷移しようとした時点を、
		 //現在ステートの完了として扱う。		 
		 //停止予約中は次の子ステートへ入らず、
		 //現在ステートのExitだけ実行する。
		 
		if (m_bStopAfterCurrentState)
		{
			Stop();
			return true;
		}
	}

	if (!IsValid(_newState))
	{
		return false;
	}

	//登録されていないステートには遷移しない
	if (!m_states.Contains(_newState))
	{
		return false;
	}

	//同じステートへの遷移は行わない
	if (m_currentState == _newState)
	{
		return false;
	}

	if (!_newState->CanEnter())
	{
		return false;
	}

	if (IsValid(m_currentState) &&
		!m_currentState->CanTransitionTo(_newState))
	{
		return false;
	}

	if (IsValid(m_currentState))
	{
		m_currentState->Exit();
	}

	m_currentState = _newState;
	m_currentState->Enter();

	return true;
}

//================================================
//ランダムでチェンジステート　引数はランダムから除外するステート
//================================================
bool UStateMachine::RandomChangeState_Exclude(TArray<UStateBase*> _excludeStates)
{
	TArray<UStateBase*> states = m_states;
	//除外する
	states.RemoveAll(
		[&_excludeStates](UStateBase*& state)
		{
			return _excludeStates.Contains(state);
		}
	);

	//ランダムの範囲
	int randMax = states.Num()-1;	
	int randIndex = -1;

	//チェンジステートがダメだった場合の保険で回す回数
	int maxTryCount = 3;
	
	for (int i = 0;i < maxTryCount;++i)
	{
		//インデックスランダムで選ぶ
		randIndex = FMath::RandRange(0, randMax);
		//インデックスが有効か
		if (!states.IsValidIndex(randIndex)) 
		{ 		
			continue; 
		}
		
		//インデックスのステートが有効か
		if (IsValid(states[randIndex]))
		{
			//ステートちぇんじできたら終了
			if (ChangeState(states[randIndex])) 
			{
				return true;
			}
		} 

		//変更できなかったので乱数の最大値下げるのととステートを配列から削除
		randMax= FMath::Clamp(randMax, 0, randMax - 1);
		states.Remove(states[randIndex]);
	}

		
	return false;
}

//==================================================
//現在ステート終了後の停止を予約
//==================================================
void UStateMachine::RequestStopAfterCurrentState()
{
	/*
	 * すでにカレントがない場合は、
	 * 停止予約を残さない。
	 */
	if (!IsValid(m_currentState))
	{
		m_bStopAfterCurrentState = false;
		return;
	}

	m_bStopAfterCurrentState = true;
}

//==================================================
//強制ステート変更
//==================================================
bool UStateMachine::ForceChangeState(UStateBase* _newState)
{
	if (!IsValid(_newState))
	{
		return false;
	}

	if (!m_states.Contains(_newState))
	{
		return false;
	}

	/*
	 * 強制遷移は停止予約より優先する。
	 * 死亡ステートなどで使用する想定。
	 */
	m_bStopAfterCurrentState = false;

	if (m_currentState == _newState)
	{
		return false;
	}

	if (IsValid(m_currentState))
	{
		m_currentState->Exit();
	}

	m_currentState = _newState;
	m_currentState->Enter();
	return true;
}

//==================================================
//ステートマシン停止処理
//==================================================
void UStateMachine::Stop()
{
	if (IsValid(m_currentState))
	{
		m_currentState->Exit();
	}
	m_currentState = nullptr;
	m_bStopAfterCurrentState = false;
}

//==================================================
//ステートマシンリセット処理
//==================================================
void UStateMachine::Reset(UStateBase* _initialState)
{
	if (IsValid(m_currentState))
	{
		m_currentState->Exit();
	}

	m_currentState = nullptr;
	m_bStopAfterCurrentState = false;

	for (UStateBase* State : m_states)
	{
		if (IsValid(State))
		{
			State->Reset();
		}
	}

	if (IsValid(_initialState))
	{
		m_currentState = _initialState;
		m_currentState->Enter();
	}
}