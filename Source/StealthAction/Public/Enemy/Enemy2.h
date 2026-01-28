//----------------------------------------------------------
// 概要				：エネミー2を制御するH
// 更新日　　　　 　：
// 担当				：24CU0237廣川菖
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "Enemy2.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API AEnemy2 : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	AEnemy2();

	// ゲームスタート時、または生成時に呼ばれる処理
	virtual void BeginPlay() override;
	//聴覚処理
	virtual void UpdateHearing(float _deltaTime)override;
	//探索処理
	virtual void UpdateSearch(float _deltaTime) override;
	//戦闘処理
	virtual void CaseBattle(float _deltaTime) override;

private:

	//見つかる範囲
	float m_foundDistance;

};
