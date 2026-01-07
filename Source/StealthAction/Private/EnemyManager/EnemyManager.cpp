// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager/EnemyManager.h"
#include "Enemy/EnemyBase.h"

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
	//エネミー配列を空に
	m_pAllEnemies.Empty();
}

//------------------------------------------------
//エネミー登録関数
//------------------------------------------------
void UEnemyManager::RegisterEnemy(AEnemyBase* _pEnemy)
{
	if (!_pEnemy)
	{
		return;
	}
	//エネミーのいるエリア番号を取得
	const int32 EriaNum = 0;/*_pEnemy->GetEriaNum();*/
	//エリア番号分の配列がないなら追加
	while (m_pAllEnemies.Num() <= EriaNum)
	{
		m_pAllEnemies.Add(TArray<AEnemyBase*>());
	}
	//エリア配列にエネミーを追加
	m_pAllEnemies[EriaNum].Add(_pEnemy);
}

//------------------------------------------------
//一番近いエネミー取得関
//------------------------------------------------
AEnemyBase* UEnemyManager::GetNearestEnemy(const FVector& _pos, const int32& _eriaNum, const float& _maxDistance) const
{
	//エリア番号が配列外ならnullptrを返す
	if (_eriaNum < 0 || m_pAllEnemies.Num() <= _eriaNum)
	{
		return nullptr;
	}
	//一番近いエネミーを探す
	//return用変数
	AEnemyBase* pNearestEnemy = nullptr;
	//最短距離保存するよう変数,初期値は最大値
	float nearestDistance = TNumericLimits<float>::Max();
	//エリア内のエネミー全てを確認
	for (AEnemyBase* pEnemy : m_pAllEnemies[_eriaNum])
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