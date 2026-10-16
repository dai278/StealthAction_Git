// Fill out your copyright notice in the Description page of Project Settings.


#include "State/StateBase.h"
#include "State/StateMachineComponent.h"

UStateBase::UStateBase()
{
}

void UStateBase::Initialize(UStateMachine* _inStateMachine, AActor* _inOwner)
{
	m_pStateMachine = _inStateMachine;

	m_pOwner = _inOwner;
}

void UStateBase::Reset()
{
	m_stateTimer = 0.f;
}

//タグを所持してるか
bool UStateBase::HasTag(FName _tag)
{
	return m_tag==_tag;
}

//========================================================
//エディタに設定されているnextStateからnextStateを取得
//========================================================
UStateBase* UStateBase::GetNextStateClassInstance()
{
	if (IsValid(m_pStateMachine)) { return nullptr; }
	if (!m_nextStateClass) { return nullptr; }

	return m_pStateMachine->GetStateBySubClass(m_nextStateClass);
}



