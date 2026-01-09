// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "Enemy1.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API AEnemy1 : public AEnemyBase
{
	GENERATED_BODY()

public:

	// ゲームスタート時、または生成時に呼ばれる処理
	virtual void BeginPlay() override;
 
	
};
