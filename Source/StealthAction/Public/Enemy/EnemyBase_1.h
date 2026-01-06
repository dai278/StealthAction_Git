// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyBase_1.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API UEnemyBase_1 : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	UEnemyBase_1();

public:

	//視界処理
	void UpdateVisiblity(ACharacter* m_pEnemy);
};
