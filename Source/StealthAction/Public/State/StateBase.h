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

	//開始処理
	virtual void Enter();
	//更新処理
	virtual void Update(const float& _deltaTime);
	//終了処理
	virtual void Exit();

	//開始可能か
	virtual bool CanEnter() const;
	//終了可能か
	virtual bool CanExit() const;
	//遷移可能か
	virtual bool CanTransitionTo(const UStateBase* _newState) const;

	virtual void Reset();

	bool HasTag(FName _tag);
	FName GetTag()const { return m_tag; }
protected:

	//開始処理(BPで上書き可能)
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnEnter();
	virtual void OnEnter_Implementation();

	//更新処理(BPで上書き可能)
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnUpdate(float DeltaTime);
	virtual void OnUpdate_Implementation(float DeltaTime);

	//終了処理(BPで上書き可能)
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnExit();
	virtual void OnExit_Implementation();

	//開始条件
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	bool CanEnterState() const;
	virtual bool CanEnterState_Implementation() const;

	//終了条件
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	bool CanExitState() const;
	virtual bool CanExitState_Implementation() const;

	//遷移条件
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	bool CanTransitionToState(TSubclassOf<UStateBase> NewStateClass) const;
	virtual bool CanTransitionToState_Implementation(TSubclassOf<UStateBase> NewStateClass) const;

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
	bool m_bUseTimer = false;

	//オーナーのポインタ
	AActor* m_pOwner;

	//終了可能フラグ
	bool m_bCanExit = true;

	UPROPERTY(EditAnywhere)
	FName m_tag="";

	UPROPERTY(EditAnywhere)
	TSubclassOf<UStateBase> m_nextStateClass=nullptr;
};
