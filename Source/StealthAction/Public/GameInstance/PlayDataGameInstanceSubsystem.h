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

private:
	//プレイヤー情報の取得
	UFUNCTION()
	//FPlayerInfo GetPlayerInfo()const;

	//BeginPlay
	void BeginPlayHandler(UWorld* World);

	//プレイデータ情報
	FPlayDataInfo m_playDataInfo;
};
