// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint/CheckpointManager.h"
#include "Checkpoint/Checkpoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/PlayDataGameInstanceSubsystem.h"


//----------------------------------------------------------
//コンストラクタ
//----------------------------------------------------------
UCheckpointManager::UCheckpointManager()
{
	//空にする
	m_checkpoints.Empty();
}


//------------------------------------------------
// BeginPlay関数
//------------------------------------------------
void UCheckpointManager::OnWorldBeginPlay(UWorld&)
{
	//インゲームレベルでなければ処理しない
	FString name = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);
	if (name == "Title") { return; }
	if (name == "GameOver") { return; }
	if (name == "GameClear") { return; }

	//空にする
	m_checkpoints.Empty();

	//プレイデータの取得
	UPlayDataGameInstanceSubsystem* pPlayData = GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>();
	if (!pPlayData)
	{
		UE_LOG(LogTemp, Display, TEXT("UPlayDataGameInstanceSubsystem Genereted Miss"));
		return;
	}
	FCheckpointInfo info= pPlayData->GetCheckpointInfo();
	m_currentCheckpointIndex = info.Index;

	//Tickの1フレーム目に呼ばれる関数
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UCheckpointManager::SetCheckPointAll);


}

//----------------------------------------------------------
//チェックポイントを登録する関数
//----------------------------------------------------------
void UCheckpointManager::AddCheckpoint(ACheckpoint* _checkpoint)
{
	if (!_checkpoint)
	{
		return;
	}

	//インデックスが不正なら登録しない
	if(_checkpoint->GetIndex() <0)
	{
		return;
	}

	//ライトを追加する要素数を特定
	for (int i = 0;i < m_checkpoints.Num(); ++i)
	{
		//この要素数のライトよりインデックスが低いなら
		//この要素数に追加する
		if (_checkpoint->GetIndex() == i)
		{
			//nullなら追加
			if (m_checkpoints[i] == nullptr) {
				//TArrayにライトを追加
				m_checkpoints[i] = _checkpoint;
				return;
			}

			UE_LOG(LogTemp, Display, TEXT("LightのIndexがかぶっています"), i);
			return;
		}

	}

	//ここまで来たら現在の要素数より大きい為要素数をライトインデックス分まで追加
	m_checkpoints.SetNum(_checkpoint->GetIndex() + 1);
	m_checkpoints[_checkpoint->GetIndex()] = _checkpoint;
}

//----------------------------------------------------------
//チェックポイントの取得関数
//----------------------------------------------------------
ACheckpoint* UCheckpointManager::GetCheckpoint(const int32 _index) const
{
	//インデックスが不正ならnullptrを返す
	if (_index < 0 || m_checkpoints.Num() <= _index)
	{
		return nullptr;
	}
	return m_checkpoints[_index];
}

//----------------------------------------------------------
//現在のチェックポイントの位置取得
//----------------------------------------------------------
FVector UCheckpointManager::GetCurrentCheckpointLocation() const
{
	
	//インゲームレベルでなければ処理しない
	FString name = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);
	if (name == "Title"||name == "GameOver"||name == "GameClear") 
	{ 
		return FVector::ZeroVector;
	}

	for (ACheckpoint* check : m_checkpoints)
	{
		if (check->GetIndex() == m_currentCheckpointIndex)
		{
			return check->GetActorLocation();
		}
	}
	return FVector::ZeroVector;
}

//----------------------------------------------------------
//現在のチェックポイントを設定する関数
//----------------------------------------------------------
void UCheckpointManager::SetCurrentCheckpointIndex(const int32& _index)
{
	//インデックスが0未満か配列外、現在のチェックポイントより小さいなら設定しない
	if (_index < 0 || _index <m_currentCheckpointIndex)
	{
		UE_LOG(LogTemp, Display, TEXT("SetCurrentCheckpointIndex,Miss"));
		return;
	}
	if(_index < m_currentCheckpointIndex)
	{
		UE_LOG(LogTemp, Display, TEXT("SetCurrentCheckpointIndex,Miss"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("index："),_index);
	//　不正な値でなければインデックスを設定
	m_currentCheckpointIndex = _index;

	//	//プレイデータの取得
	UPlayDataGameInstanceSubsystem* pPlayData = GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>();
	if (!pPlayData)
	{
		UE_LOG(LogTemp, Display, TEXT("UPlayDataGameInstanceSubsystem Genereted Miss"));
		return;
	}

	pPlayData->SetCheckpointInfo(m_checkpoints[_index]->GetCheckpointInfo());

}


//----------------------------------------------------------
//現在のチェックポイントインデックスを取得する関数
//----------------------------------------------------------
int32 UCheckpointManager::GetCurrentCheckpointIndex() const
{
	return m_currentCheckpointIndex;
}

//-----------------------------------------------------------
//チェックポイントに触れた際に呼び出す関数
//-----------------------------------------------------------
void UCheckpointManager::OnOverlapCheckpoint()
{
	UE_LOG(LogTemp, Display, TEXT("OnOverlapCheckpoint"));
	int32 num = m_OnOverlapCheckpointFunctions.Num();
	for (std::function<void()>& func : m_OnOverlapCheckpointFunctions)
	{
		if (func) { func(); }
	}
}

//---------------------------------------------------------
//全チェックポイントの設定
//---------------------------------------------------------
void UCheckpointManager::SetCheckPointAll()
{
	//プレイデータの取得
	UPlayDataGameInstanceSubsystem* pPlayData = GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>();
	if (!pPlayData) { return; }
	if (!pPlayData->IsIsContinued()) {
		return;
	}

	for (ACheckpoint* check : m_checkpoints)
	{
		//現在のインデックス以下は非アクティブに
		if (check->GetIndex() <= m_currentCheckpointIndex)
		{
			check->SetHasCheckpoint(false);
		}
	}

}