//----------------------------------------------------------------
// エネミーマネージャークラス
//----------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyManager.generated.h"

//前方宣言
class AEnemy_1;//エネミーの基底クラス


UCLASS()
class STEALTHACTION_API UEnemyManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//コンストラクタ
	UEnemyManager();

	//BeginPlay関数
	virtual void OnWorldBeginPlay(UWorld&)override;

public:
	//エネミー登録関数
	void RegisterEnemy(AEnemy_1* _pEnemy);
	
	//一番近いエネミー取得関数
	AEnemy_1* GetNearestEnemy(const FVector& _pos,const int32& _eriaNum,const float& _maxDistance) const;
	
	////エネミー全取得関数
	//const TArray<AEnemy_1*>& GetAllEnemy() const { return m_pAllEnemy_1; }

private:
	//エネミー配列
	//　m_pAllEnemies[敵のいるエリア][そのエリアの敵全員]
	TArray<TArray<AEnemy_1*>> m_pAllEnemies;
	
};
