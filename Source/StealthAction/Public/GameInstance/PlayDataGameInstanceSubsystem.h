// Fill out your copyright notice in the Description page of Project Settings.
//--------------------------------------------
// プレイデータ管理用GameInstanceSubsystem
//------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayDataInfo.h" 

#include "PlayDataGameInstanceSubsystem.generated.h"


UCLASS()
class STEALTHACTION_API UPlayDataGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//ゲーム起動時一度のみ呼ばれる初期化処理
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//ゲーム終了時一度のみ呼ばれる終了処理
	virtual void Deinitialize() override;
	
	//プレイヤーの情報登録
	void SetPlayerInfo(const FPlayerInfo _playInfo) { m_playDataInfo.PlayerInfo = _playInfo; }
	
	//エネミー情報の登録
	void SetEnemyInfo(const TArray<FEnemyInfo> _enemyInfo)
	{
		//一度リセット
		m_playDataInfo.EnemyInfo.Empty();
		m_playDataInfo.EnemyInfo = _enemyInfo;
	}

	//クリアしたステージ


	//プレイヤー情報の取得
	UFUNCTION()
	FPlayerInfo GetPlayerInfo()const { return m_playDataInfo.PlayerInfo; }//プレイヤー情報の取得
	FEnemyInfo GetEnemyInfo(const int32 _index)const;//エネミー情報の取得 


private:

	//BeginPlay
	void BeginPlayHandler(UWorld* World);

	//プレイデータ情報
	FPlayDataInfo m_playDataInfo;
};
