//担当日髙
//----------------------------------------------------------
//概要：ステートの共通処理
//----------------------------------------------------------

#include "State/StateBase.h"
#include "State/StateMachineComponent.h"

UStateBase::UStateBase()
{
}

//-----------------------------------------------------
//初期化
//-----------------------------------------------------
void UStateBase::Initialize(UStateMachine* _inStateMachine, AActor* _inOwner)
{
	m_pStateMachine = _inStateMachine;

	m_pOwner = _inOwner;
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UStateBase::Enter()
{
	//BPで親を呼ばなくてもタイマーは初期化する
	m_stateTimer = 0.f;
	m_bUseTimer = true;
	OnEnter();
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UStateBase::Update(const float& _deltaTime)
{
	if (m_bUseTimer)
	{
		m_stateTimer += _deltaTime;
	}
	OnUpdate(_deltaTime);
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UStateBase::Exit()
{
	OnExit();
}

//-----------------------------------------------------
//開始可能か
//-----------------------------------------------------
bool UStateBase::CanEnter() const
{
	return CanEnterState();
}

//-----------------------------------------------------
//終了可能か
//-----------------------------------------------------
bool UStateBase::CanExit() const
{
	return m_bCanExit && CanExitState();
}

//-----------------------------------------------------
//遷移可能か
//-----------------------------------------------------
bool UStateBase::CanTransitionTo(const UStateBase* _newState) const
{
	return IsValid(_newState) && CanTransitionToState(_newState->GetClass());
}

//-----------------------------------------------------
//BPで上書きしない場合の共通処理
//-----------------------------------------------------
void UStateBase::OnEnter_Implementation()
{
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UStateBase::OnUpdate_Implementation(float DeltaTime)
{
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UStateBase::OnExit_Implementation()
{
}

//-----------------------------------------------------
//開始条件
//-----------------------------------------------------
bool UStateBase::CanEnterState_Implementation() const
{
	return true;
}

//-----------------------------------------------------
//終了条件
//-----------------------------------------------------
bool UStateBase::CanExitState_Implementation() const
{
	return true;
}

//-----------------------------------------------------
//遷移条件
//-----------------------------------------------------
bool UStateBase::CanTransitionToState_Implementation(TSubclassOf<UStateBase> NewStateClass) const
{
	return true;
}

//-----------------------------------------------------
//リセット処理
//-----------------------------------------------------
void UStateBase::Reset()
{
	m_stateTimer = 0.f;
}

//タグを所持してるか
bool UStateBase::HasTag(FName _tag)
{
	return m_tag == _tag;
}

//========================================================
//エディタに設定されているnextStateからnextStateを取得
//========================================================
UStateBase* UStateBase::GetNextStateClassInstance()
{
	if (!IsValid(m_pStateMachine))
	{
		return nullptr;
	}
	if (!m_nextStateClass)
	{
		return nullptr;
	}

	return m_pStateMachine->GetStateBySubClass(m_nextStateClass);
}
