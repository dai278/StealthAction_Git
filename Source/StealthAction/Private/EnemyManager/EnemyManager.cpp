// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager/EnemyManager.h"
#include "Enemy/EnemyBase.h"
#include "GameInstance/PlayDataGameInstanceSubsystem.h"
#include "Checkpoint/CheckpointManager.h"
#include "Kismet/GameplayStatics.h"

//------------------------------------------------
// コンストラクタ
//------------------------------------------------
UEnemyManager::UEnemyManager()
{

}

//------------------------------------------------
// BeginPlay関数
//------------------------------------------------
void UEnemyManager::OnWorldBeginPlay(UWorld& )
{
	//インゲームレベルでなければ処理しない
	FString name = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);
	if (name == "Title") { return; }
	if (name == "GameOver") { return; }
	if (name == "GameClear") { return; }


	//エネミー配列を空に
	m_pAllEnemies.Empty();
	//チェックポイントマネージャ―に関数を登録
	UCheckpointManager* pCheckMng = GetWorld()->GetSubsystem<UCheckpointManager>();
	if (pCheckMng)
	{
		pCheckMng->AddOnOverlapCheckpointEvent(CreateOverlapCheckpointEvent(UEnemyManager::RegisterEnemyInfoAllToPlayData));
	}

	//Tickの1フレーム目に呼ばれる関数
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEnemyManager::SetEnemyInfoAll);

}

//------------------------------------------------
//エネミー登録関数
//------------------------------------------------
void UEnemyManager::RegisterEnemy(AEnemyBase* _pEnemy)
{
	m_pAllEnemies.Add(_pEnemy);
	if (!_pEnemy)
	{
		UE_LOG(LogTemp, Display, TEXT("Enemy->null"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Enemy->NotNull"));
	}
}

//------------------------------------------------
//全エネミー情報をPlayDataに記録する
//------------------------------------------------
void UEnemyManager::RegisterEnemyInfoAllToPlayData()
{

	//インゲームレベルでなければ処理しない
	FString name = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);
	if (name == "Title") { return; }
	if (name == "GameOver") { return; }
	if (name == "GameClear") { return; }

	//プレイデータの取得
	UPlayDataGameInstanceSubsystem* pPlayData = GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>();
	if (!pPlayData)
	{
		UE_LOG(LogTemp, Display, TEXT("UPlayDataGameInstanceSubsystem Genereted Miss"));
		return;
	}

	TArray<FEnemyInfo> enemyInfos;
	//全エネミーからenemyInfoを取得
	for (AEnemyBase* pEnemy : m_pAllEnemies)
	{
		enemyInfos.Add(pEnemy->GetEnemyInfo());
	}
	//プレイデータに記録
	pPlayData->SetEnemyInfo(enemyInfos);
}

//------------------------------------------------
//一番近いエネミー取得関
//------------------------------------------------
AEnemyBase* UEnemyManager::GetNearestEnemy(const FVector& _pos,  const float& _maxDistance) const
{

	//一番近いエネミーを探す
	//return用変数
	AEnemyBase* pNearestEnemy = nullptr;
	//最短距離保存するよう変数,初期値は最大値
	float nearestDistance = TNumericLimits<float>::Max();
	//エリア内のエネミー全てを確認
	for (AEnemyBase* pEnemy : m_pAllEnemies)
	{
		if (!pEnemy) { continue; }
		//エネミーと指定座標の距離を計算
		const float distance = FVector::Dist(pEnemy->GetActorLocation(), _pos);
		//最短距離より近く検知の最大距離より近ければ更新
		if (distance < nearestDistance)
		{
			nearestDistance = distance;
			if (distance < _maxDistance) {
				pNearestEnemy = pEnemy;
			}
		}
	}
	return pNearestEnemy;
}


//----------------------------------------------
//前エネミーにEnemyInfoを登録する関数
//----------------------------------------------
void UEnemyManager::SetEnemyInfoAll()
{
	//プレイデータの取得
	UPlayDataGameInstanceSubsystem* pPlayData = GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>();
	if (!pPlayData) { return; }
	if (!pPlayData->IsIsContinued()) { 
		return;
	}
	//コンテニューしていれば
	int32 num = m_pAllEnemies.Num();
	
	TArray<FEnemyInfo> info = pPlayData->GetAllEnemyInfo();
	int32 infoNum = info.Num();
	if (num != infoNum) { return; }
	UE_LOG(LogTemp, Warning, TEXT(""), num);

	//インデックスが同じエネミーに情報設定
	for (int i = 0;i < m_pAllEnemies.Num();++i)
	{
		for (int n = 0;n < info.Num();++n)
		{
			if (m_pAllEnemies[i]->GetEnemyInfo().index == info[n].index)
			{
				UE_LOG(LogTemp, Warning, TEXT("Before: i=%d Num=%d"), i, m_pAllEnemies.Num());
				//情報設定
				m_pAllEnemies[i]->SetEnemyInfo(info[n]);
				//死んでる敵は死んだ位置にリスポーン
				//生きているのは初期位置
				if (m_pAllEnemies[i]->GetEnemyInfo().bIsDead) {
					m_pAllEnemies[i]->SetActorLocation(info[n].location);
				}
				UE_LOG(LogTemp, Warning, TEXT("After : i=%d Num=%d"), i, m_pAllEnemies.Num());
				break;
			}
		}
	}
}

