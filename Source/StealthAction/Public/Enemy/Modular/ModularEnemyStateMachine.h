//----------------------------------------------------------
//概要：敵のステート管理
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/StateMachineComponent.h"
#include "ModularEnemyStateMachine.generated.h"

class UModularEnemyState;

//大きな状態の切り替えだけを管理する
UCLASS(Blueprintable)

class STEALTHACTION_API UModularEnemyStateMachine : public UStateMachine
{
	GENERATED_BODY()

  public:

	//初期化
	virtual void Initialize(AActor* Owner) override;

	//更新処理
	virtual void Update(float DeltaTime) override;

	//停止処理
	virtual void Stop() override;

	//被ダメージと死亡はBTの判断より優先
	void NotifyDamage();

	//現在の状況で探索か戦闘へ戻す
	bool ReturnToAction();

	//敵ステート変更
	UFUNCTION(BlueprintCallable, Category = "Enemy State")
	bool ChangeEnemyState(TSubclassOf<UModularEnemyState> StateClass);

	//登録が完了しているか
	UFUNCTION(BlueprintPure, Category = "Enemy State")

	bool IsConfigured() const { return bConfigured; }

  private:

	//ステートの登録が完了したか
	bool bConfigured = false;

	//ステート変更中か
	bool bChangingState = false;
};
