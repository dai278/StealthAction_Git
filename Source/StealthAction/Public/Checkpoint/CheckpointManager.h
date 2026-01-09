// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CheckpointManager.generated.h"

//クラスのメンバ関数をノイズイベントのコールバックにするときのマクロ
#define CreateOverlapCheckpointEvent(_func) std::bind(&_func, this)

//前方宣言
class ACheckpoint;

/**
 * 
 */
UCLASS()
class STEALTHACTION_API UCheckpointManager : public UWorldSubsystem
{
	GENERATED_BODY()

	//コンストラクタ
	UCheckpointManager();

	//BeginPlay関数
	virtual void OnWorldBeginPlay(UWorld&)override;
	
public:
	//チェックポイントを登録する関数
	void AddCheckpoint(ACheckpoint* _checkpoint);
	
	//チェックポイントの取得関数
	ACheckpoint* GetCheckpoint(const int32 _index) const;

	//現在のチェックポイントインデックスを設定する関数
	FVector GetCurrentCheckpointLocation() const;
	//現在のチェックポイントを設定する関数
	void SetCurrentCheckpointIndex(const int32& _index);
	//現在のチェックポイントインデックスを取得する関数
	int32 GetCurrentCheckpointIndex() const;
	//チェックポイント触れた際のイベント関数の追加
	void AddOnOverlapCheckpointEvent(std::function<void()> _func)
	{
		if (_func) { m_OnOverlapCheckpointFunctions.Add(_func);}
	}

	//チェックポイントに振れた際呼び出す関数
	void OnOverlapCheckpoint();

	//全チェックポイントの設定
	void SetCheckPointAll();


private:

	TArray<ACheckpoint*> m_checkpoints;//チェックポイントの配列
	
	int32 m_currentCheckpointIndex;//現在のチェックポイントインデックス

	TArray<std::function<void()>> m_OnOverlapCheckpointFunctions; //イベント関数配列
};
