// Fill out your copyright notice in the Description page of Project Settings.

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"			//ItemBaseクラス継承
#include "ItemBase.h"
#include "ItemKey.generated.h"

class AGoalActor;

UCLASS()
class STEALTHACTION_API AItemKey : public AItemBase
{
	//UEのオブジェクトクラスで必ず先頭に書くマクロ
	GENERATED_BODY()

public:
	//コンストラクタ
	AItemKey();

protected:
	// オーバーラップ時
	virtual void HandleOverlap(AActor* OtherActor) override;

	// ===== ゴール生成用 =====
	UPROPERTY(EditAnywhere, Category = "Goal")
	TSubclassOf<AGoalActor> GoalClass;

	UPROPERTY(EditAnywhere, Category = "Goal")
	FVector GoalSpawnLocation;
};
