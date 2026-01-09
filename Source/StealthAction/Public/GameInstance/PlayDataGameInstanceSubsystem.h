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
	UFUNCTION(BlueprintCallable, Category = "PlayData|Player")
	void SetPlayerInfo(const FPlayerInfo _playInfo) { m_playDataInfo.PlayerInfo = _playInfo; }
	//プレイヤー情報の取得
	UFUNCTION(BlueprintPure, Category = "PlayData|Player")
	FPlayerInfo GetPlayerInfo()const { return m_playDataInfo.PlayerInfo; }
	//プレイヤー情報のリセット
	UFUNCTION(BlueprintCallable, Category = "PlayData|Player")
	void ResetPlayerInfo() { m_playDataInfo.PlayerInfo = FPlayerInfo(); }
		
	//エネミー情報の登録
	UFUNCTION(BlueprintCallable, Category = "PlayData|Enemy")
	void SetEnemyInfo(const TArray<FEnemyInfo> _enemyInfo)
	{
		//一度リセット
		m_playDataInfo.EnemyInfo.Empty();
		m_playDataInfo.EnemyInfo = _enemyInfo;
	}
	
	//エネミー情報の取得
	UFUNCTION(BlueprintPure, Category = "PlayData|Enemy")
	FEnemyInfo GetEnemyInfo(const int32 _index)const;
	//全エネミー情報の取得
	TArray<FEnemyInfo> GetAllEnemyInfo()const { return m_playDataInfo.EnemyInfo; }
	
	//エネミー情報のリセット
	UFUNCTION(BlueprintCallable, Category = "PlayData|Enemy")
	void ResetEnemyInfo() { m_playDataInfo.EnemyInfo.Empty(); }

	//クリアしたステージ
	UFUNCTION()
	void SetClearedStage(const FName& _levelName){m_playDataInfo.ClearLevelNames.Add(_levelName);}

	
	//チェックポイント情報の更新
	UFUNCTION()
	void SetCheckpointInfo(const FCheckpointInfo& _checkpointInfo) { m_playDataInfo.LastCheckpointInfo = _checkpointInfo; }
	//チェックポイント情報の取得
	UFUNCTION()
	FCheckpointInfo GetCheckpointInfo()const { return m_playDataInfo.LastCheckpointInfo; }
	//チェックポイントのリセット
	UFUNCTION()
	void ResetCheckpointInfo() { m_playDataInfo.LastCheckpointInfo = FCheckpointInfo(); }


	//クリアしたステージの取得
	UFUNCTION()
	TArray<FName> GetClearedStages()const { return m_playDataInfo.ClearLevelNames; }
	//指定したステージがクリア済みか//Contains関数はTArrayに指定した要素が含まれているかを調べる関数
	UFUNCTION()
	bool IsStageCleared(const FName& _levelName)const { return m_playDataInfo.ClearLevelNames.Contains(_levelName); }
	//クリアしたステージを追加
	UFUNCTION()
	void AddClearedStage(const FName& _levelName) { m_playDataInfo.ClearLevelNames.Add(_levelName); }	
	//クリアしたステージをリセット
	UFUNCTION()
	void ResetClearedStages() { m_playDataInfo.ClearLevelNames.Empty(); }


	//プレイデータ情報の取得
	UFUNCTION()
	FPlayDataInfo GetPlayDataInfo()const { return m_playDataInfo; }
	//プレイデータ情報の設定
	UFUNCTION()
	void SetPlayDataInfo(const FPlayDataInfo& _playDataInfo) { m_playDataInfo = _playDataInfo; }
	
	//プレイデータ情報のリセット
	UFUNCTION(BlueprintCallable, Category = "PlayData")
	void ResetPlayDataInfo() { m_playDataInfo = FPlayDataInfo(); }

	//ステージクリア時の処理
	UFUNCTION(BlueprintCallable, Category = "PlayData")
	void OnStageClear(const FName& _levelName);

	//ステージセレクトに戻る時の処理
	UFUNCTION(BlueprintCallable, Category = "PlayData")
	void OnReturnToStageSelect();

	//コンテニュー時の処理
	UFUNCTION(BlueprintCallable, Category = "PlayData")
	void OnContinue();

	//コンテニューしたか？
	UFUNCTION(BlueprintCallable, Category = "PlayData")
	bool IsIsContinued()const { return m_playDataInfo.IsContinued; }
private:

	//BeginPlay
	void BeginPlayHandler(UWorld* World);

	//プレイデータ情報
	FPlayDataInfo m_playDataInfo;
};
