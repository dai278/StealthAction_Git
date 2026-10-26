//担当日髙
//----------------------------------------------------------
//概要：ステートの共通処理
//----------------------------------------------------------

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

	//初期化
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

	//リセット処理
	virtual void Reset();

	//タグを所持しているか
	bool HasTag(FName _tag);

	//タグ取得
	FName GetTag() const { return m_tag; }

  protected:


	//開始処理(BPで上書き可能)
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnEnter();

	//開始処理
	virtual void OnEnter_Implementation();

	//更新処理(BPで上書き可能)
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnUpdate(float DeltaTime);

	//更新処理
	virtual void OnUpdate_Implementation(float DeltaTime);

	//終了処理(BPで上書き可能)
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	void OnExit();

	//終了処理
	virtual void OnExit_Implementation();

	//開始条件
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	bool CanEnterState() const;

	//開始条件
	virtual bool CanEnterState_Implementation() const;

	//終了条件
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	bool CanExitState() const;

	//終了条件
	virtual bool CanExitState_Implementation() const;

	//遷移条件
	UFUNCTION(BlueprintNativeEvent, Category = "State")
	bool CanTransitionToState(TSubclassOf<UStateBase> NewStateClass) const;

	//遷移条件
	virtual bool CanTransitionToState_Implementation(TSubclassOf<UStateBase> NewStateClass) const;

	//次のステート取得
	UStateBase* GetNextStateClassInstance();

  public:


	//次のステート
	UPROPERTY()
	UStateBase* m_nextState = nullptr;

  protected:

	//所属するステートマシン
	UPROPERTY()
	TObjectPtr<UStateMachine> m_pStateMachine = nullptr;

	//ステートの時間
	float m_stateTimer = 0.f;

	//時間を計測するか
	bool m_bUseTimer = false;

	//オーナーのポインタ
	AActor* m_pOwner;

	//終了可能フラグ
	bool m_bCanExit = true;

	//ステートのタグ
	UPROPERTY(EditAnywhere)
	FName m_tag = "";

	//次のステートのクラス
	UPROPERTY(EditAnywhere)
	TSubclassOf<UStateBase> m_nextStateClass = nullptr;
};
