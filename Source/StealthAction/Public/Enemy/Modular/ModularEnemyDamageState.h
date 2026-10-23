#pragma once

#include "CoreMinimal.h"
#include "Enemy/Modular/ModularEnemyState.h"
#include "ModularEnemyDamageState.generated.h"

//被ダメージステート
UCLASS(Blueprintable)
class STEALTHACTION_API UModularEnemyDamageState : public UModularEnemyState
{
	GENERATED_BODY()
protected:
	//ひるみから復帰するまでの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy State", meta = (ClampMin = "0.0"))
	float RecoveryTime = 0.4f;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
};
