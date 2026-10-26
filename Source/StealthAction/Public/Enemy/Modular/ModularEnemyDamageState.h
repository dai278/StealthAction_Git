//担当日髙
//----------------------------------------------------------
//概要：被ダメージステート
//----------------------------------------------------------

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

	//開始処理
	virtual void OnEnter_Implementation() override;

	//更新処理
	virtual void OnUpdate_Implementation(float DeltaTime) override;
};
