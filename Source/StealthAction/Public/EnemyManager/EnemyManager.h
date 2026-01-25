//----------------------------------------------------------------
// エネミーマネージャークラス
//----------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyManager.generated.h"

//前方宣言
class AEnemyBase;//エネミーの基底クラス


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
	void RegisterEnemy(AEnemyBase* _pEnemy);
	
	//一番近いエネミー取得関数
	AEnemyBase* GetNearestEnemy(const FVector& _pos,const float& _maxDistance) const;
	
	//全エネミーの情報をPlayDataに記録する
	void RegisterEnemyInfoAllToPlayData();

	//エネミーがプレイヤー発見しているか取得する関数
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	bool IsAnyEnemyPlayerFound() const;

	////エネミー全取得関数
	//const TArray<EnemyBase*>& GetAllEnemy() const { return m_pAllEnemy_1; }

private:

	//全エネミーにエネミー情報を登録する関数
	void SetEnemyInfoAll();

	//エネミー配列
	//　m_pAllEnemies[敵のいるエリア][そのエリアの敵全員]
	TArray<AEnemyBase*> m_pAllEnemies;
	
};
