//担当日髙
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "State/StateBase.h"
#include "StateMachineComponent.generated.h"

class UStateBase;

UCLASS(BlueprintType, Blueprintable)
class STEALTHACTION_API UStateMachine : public UObject
{
	GENERATED_BODY()

public:
	//コンストラクタ
	UStateMachine();

public:

	//初期化
	virtual void Initialize(AActor* _inOwner);

public:

	//更新処理
	virtual void Update(float _deltaTime);

public:

	//ステート登録
	template<typename T>
	T* RegisterState()
	{
		//型チェック
		static_assert(TIsDerivedFrom<T, UStateBase>::IsDerived,
			"T must derive from UStateBase");

		//既に登録されている場合は登録済みのステートを返す
		if (GetState<T>())
		{
			return GetState<T>();
		}

		T* State = NewObject<T>(this);

		m_states.Add(State);

		return State;

	}

	//ステート取得
	template<typename T>
	T* GetState() const
	{
		for (UStateBase* state : m_states)
		{
			if (!IsValid(state)) { continue; }
			if (T* casted = Cast<T>(state))
			{
				return casted;
			}
		}

		return nullptr;
	}

	//現在のステートが指定した型か
	template<typename T>
	bool IsCurrentState() const
	{
		return IsValid(m_currentState) && m_currentState->template IsA<T>();
	}

	//ステート取得(Tag版)
	UStateBase* GetState(const FName _tag);

	UStateBase* GetStateBySubClass(TSubclassOf<UStateBase> TargetClass);

	//現在ステート取得
	UStateBase* GetCurrentState() const { return IsValid(m_currentState) ? m_currentState.Get() : nullptr; }

	//ステートチェンジ
	bool ChangeState(UStateBase* _newState);
	//ランダムでステートチェンジ
	bool RandomChangeState_Exclude(TArray<UStateBase*> _excludeStates);

	//現在ステート終了後に停止を予約
	void RequestStopAfterCurrentState();

	//強制ステート変更
	bool ForceChangeState(UStateBase* _newState);

	virtual void Stop();

	void Reset(UStateBase* _initialState);

	//オーナー取得
	AActor* GetOwner() const { return m_owner; }

	void Start();

protected:
	const TArray<TObjectPtr<UStateBase>>& GetRegisteredStates() const { return m_states; }

private:

	UPROPERTY()
	TArray<TObjectPtr<UStateBase>> m_states;	   //登録ステート

	UPROPERTY(EditDefaultsOnly, Category = "State Machine", meta = (DisplayName = "State Classes", AllowAbstract = "false"))
	TArray<TSubclassOf<UStateBase>> m_stateClasses; //登録するステートクラスズ

	UPROPERTY()
	TObjectPtr<UStateBase> m_currentState = nullptr; //現在ステート

	UPROPERTY()
	TObjectPtr<AActor> m_owner = nullptr;				//オーナー

	//現在ステート終了後に停止するか
	bool m_bStopAfterCurrentState = false;
};