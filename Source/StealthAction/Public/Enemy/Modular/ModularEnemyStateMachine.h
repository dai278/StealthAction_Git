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
	virtual void Initialize(AActor* Owner) override;
	virtual void Update(float DeltaTime) override;
	virtual void Stop() override;
	//被ダメージと死亡はBTの判断より優先
	void NotifyDamage();
	//現在の状況で探索か戦闘へ戻す
	bool ReturnToAction();
	UFUNCTION(BlueprintCallable, Category = "Enemy State")
	bool ChangeEnemyState(TSubclassOf<UModularEnemyState> StateClass);
	UFUNCTION(BlueprintPure, Category = "Enemy State")
	bool IsConfigured() const { return bConfigured; }
private:
	bool bConfigured = false;
	bool bChangingState = false;
};
