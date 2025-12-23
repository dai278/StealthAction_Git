
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_Weapon_1.generated.h"

class AEnemy_BulletStorage_1;

UCLASS()
class STEALTHACTION_API AEnemy_Weapon_1 : public AActor
{
    GENERATED_BODY()

public:
    AEnemy_Weapon_1();

    void BulletFire(float Time, AActor* Caller);

    AEnemy_BulletStorage_1* m_bulletPool;


protected:
    // ゲームスタート時、または生成時に呼ばれる処理
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, Category = "Amo")
    int m_maxAmo;	//弾倉

    int m_usedAmo;   //使用した弾数

    UPROPERTY(EditAnywhere, Category = "Amo")
    float m_reloadTime_Limit;	//リロードタイム

    float m_reloadTime;	//リロードタイム計算用

    UPROPERTY(EditAnywhere, Category = "Amo")
    float m_shotTime_Limit;	//銃をうつ間隔

    float m_shotTime;	//銃を撃つ間隔計算用

    float m_recordTime; //先ほど呼ばれた時間を記録するためのもの

};