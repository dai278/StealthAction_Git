//----------------------------------------------------------
// 概要				：エネミー1を制御するオブジェクト
// 更新日　　　　 　：
// 担当				：24CU0237廣川菖
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "Enemy1.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API AEnemy1 : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	// コンストラクタ
	AEnemy1();

protected:
	// ゲームスタート時、または生成時に呼ばれる処理
	virtual void BeginPlay() override;
//
//public:
};
