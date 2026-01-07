// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/PlayDataGameInstanceSubsystem.h"
#include "Enemy/EnemyBase.h"

void UPlayDataGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// 初期化処理
}

void UPlayDataGameInstanceSubsystem::Deinitialize()
{
	// 終了処理
	Super::Deinitialize();
}

void UPlayDataGameInstanceSubsystem::BeginPlayHandler(UWorld* World)
{
	// ワールド開始時の処理
}


//エネミー情報取得
FEnemyInfo UPlayDataGameInstanceSubsystem::GetEnemyInfo(const int32 _index)const
{
	
	int num = m_playDataInfo.EnemyInfo.Num();
	for (int i = 0; i < num; ++i)
	{
		if (m_playDataInfo.EnemyInfo[i].index == _index)
		{
			return m_playDataInfo.EnemyInfo[i];
		}
	}
	//インデックスがそろうのがなければ初期状態を返す
	return FEnemyInfo{};
}