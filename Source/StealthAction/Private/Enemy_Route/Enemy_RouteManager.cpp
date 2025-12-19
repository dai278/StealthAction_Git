// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Route/Enemy_RouteManager.h"
#include "Kismet/GameplayStatics.h"

//新しく追加したルート
#include "Enemy_Route/Enemy_PatrolRoute.h"

UEnemy_RouteManager::UEnemy_RouteManager()
{
}

////Beginの後に呼ばれる（Actorが生成された後）
void UEnemy_RouteManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	//ルートを増やす際、下の｛　｝内の処理を増殖してから変更。
	//ルートを増やす際の変更内容
	// 	m_pEnemy_PatrolRoute_1を.hファイルで作った変数に変える。
	// 
	{
		//ルート１の確保
		TArray<AActor*> PatrolRoutes;
		UGameplayStatics::GetAllActorsOfClass(this, AEnemy_PatrolRoute::StaticClass(), PatrolRoutes);

		for (AActor* actor : PatrolRoutes)
		{
			if (AEnemy_PatrolRoute* Route = Cast<AEnemy_PatrolRoute>(actor))
			{
				m_pEnemy_PatrolRoute.Add(Route);
			}
		}
	}
}

//ルート情報を渡す
TArray<AActor*> UEnemy_RouteManager::AddRoute(int _routeNum, bool _randomRouteCheck)
{

	int Index = 0;
	TArray<AActor*> Enemy_Route = TArray<AActor*>();	//エネミーのルートポインタ（順序変えよう）

	//ルート
	for (int i = 0; i < m_pEnemy_PatrolRoute.Num(); ++i)
	{
		AEnemy_PatrolRoute* Enemy_PatrolRoute = Cast<AEnemy_PatrolRoute>(m_pEnemy_PatrolRoute[i]);

		//ルートの決定
		if (_routeNum == Enemy_PatrolRoute->m_route_Num)
		{
			UE_LOG(LogTemp, Warning, TEXT("UEnemy_RouteManager%d"), Enemy_PatrolRoute->m_route_Num);

			Index += 1;
			Enemy_Route.SetNum(Index);	//要素数のセット
			Enemy_Route[Index - 1] = Cast<AEnemy_PatrolRoute>(m_pEnemy_PatrolRoute[i]);	//順番変更用のポインターにポインターの追加
		}
	}

	m_pEnemy_PatrolRoute_Keep.SetNum(Index);	//

	if (Enemy_Route == TArray<AActor*>())
	{
		return TArray<AActor*>();
	}
	else
	{
		//指定ルート順に入れる
		for (int i = 0; i < Index; ++i)
		{
			AEnemy_PatrolRoute* Route = Cast<AEnemy_PatrolRoute>(Enemy_Route[i]);
			int Num = Route->m_route_ActionOrder - 1;
			//if(Num == )
			m_pEnemy_PatrolRoute_Keep[Num] = Enemy_Route[i];
		}
		return m_pEnemy_PatrolRoute_Keep;

	}
}
