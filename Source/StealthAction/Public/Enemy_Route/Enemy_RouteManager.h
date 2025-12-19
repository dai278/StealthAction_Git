// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Enemy_RouteManager.generated.h"

/**
 *
 */
UCLASS()
class STEALTHACTION_API UEnemy_RouteManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//コンストラクタ
	UEnemy_RouteManager();

protected:

	//virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld)override;
public:

	//作ったルートの名前のポインタ	
	TArray<AActor*> m_pEnemy_PatrolRoute;	//エネミーのルートポインタ1
	TArray<AActor*> m_pEnemy_PatrolRoute_Keep;	//エネミーのルートポインタ1

	//ルート設定
	TArray<AActor*> AddRoute(int _routeNum, bool _randomRouteCheck);

};
