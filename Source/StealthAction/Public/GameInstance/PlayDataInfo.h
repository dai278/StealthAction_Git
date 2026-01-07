#pragma once
#include "CoreMinimal.h"
#include "PlayDataInfo.generated.h"

//プレイヤー構造多
USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	int32 hp = 100; // 体力

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	bool isAlive = true; // 生存しているか


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	bool bIsGetKeyItem = false; // 鍵アイテムを取得しているか
};

//チェックポイント構造体
USTRUCT(BlueprintType)
struct FCheckpointInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	bool bHasCheckpoint = false; // チェックポイントを使用するか

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	FName LevelName; // レベルの名前

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	FName CheckpointId; // ID
};

//エネミー情報構造体
USTRUCT(BlueprintType)
struct FEnemyInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	int32 index=-1;//エネミーインデックス

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	int32 hp=100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	bool bIsDead=false;	
};

USTRUCT(BlueprintType)
struct FPlayDataInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	FPlayerInfo PlayerInfo; // プレイヤー情報

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	TArray< FEnemyInfo> EnemyInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	FCheckpointInfo CheckpointInfo; // チェックポイント情報

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayData")
	TArray<FName> ClearLevelNames; // クリアしたレベル名
};
