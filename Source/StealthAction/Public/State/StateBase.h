// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StateBase.generated.h"

class UStateMachine;

UCLASS(Abstract, BlueprintType, Blueprintable)
class STEALTHACTION_API UStateBase : public UObject
{
	GENERATED_BODY()
public:
	UStateBase();

	virtual void Initialize(UStateMachine* _inStateMachine, AActor* _inOwner);

	virtual void Enter()
	{

		m_stateTimer = 0.f;
		m_bUseTimer = true;
	}

	virtual void Update(const float& _deltaTime)
	{
		if (m_bUseTimer)
		{
			m_stateTimer += _deltaTime;
		}
	}
	virtual void Exit() {}

	virtual bool CanEnter()const { return true; }
	virtual bool CanExit()const { return m_bCanExit; }
	virtual bool CanTransitionTo(const UStateBase* _newState)const { return true; }

	virtual void Reset();

	bool HasTag(FName _tag);
	FName GetTag()const { return m_tag; }
protected:

	UStateBase* GetNextStateClassInstance();

public:

	//次のステート
	UPROPERTY()
	UStateBase* m_nextState = nullptr;

protected:
	UPROPERTY()
	TObjectPtr<UStateMachine> m_pStateMachine = nullptr;

	//ステートの時間
	float m_stateTimer = 0.f;
	bool m_bUseTimer;

	//オーナーのポインタ
	AActor* m_pOwner;

	//終了可能フラグ
	bool m_bCanExit = true;

	UPROPERTY(EditAnywhere)
	FName m_tag="";

	UPROPERTY(EditAnywhere)
	TSubclassOf<UStateBase> m_nextStateClass=nullptr;
};
