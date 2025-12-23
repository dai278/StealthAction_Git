// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Weapon/Enemy_Weapon_1.h"
#include "Enemy_Weapon/Enemy_Bullet/Enemy_BulletStorage_1.h"
#include "Enemy_Weapon/Enemy_Bullet/Enemy_Bullet_1.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

AEnemy_Weapon_1::AEnemy_Weapon_1()
    : m_maxAmo(30)
    , m_usedAmo(0)
    , m_reloadTime_Limit(5.)
    , m_shotTime_Limit(0.2)
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = Mesh;

    // Mesh 作成
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
}
void AEnemy_Weapon_1::BeginPlay()
{
    m_bulletPool = Cast<AEnemy_BulletStorage_1>(UGameplayStatics::GetActorOfClass(this, AEnemy_BulletStorage_1::StaticClass()));

    if (m_bulletPool)
    {
        UE_LOG(LogTemp, Error, TEXT("Weapon BeginPlay: BulletStorage FOUND"));
    }

    if (!m_bulletPool)
    {
        UE_LOG(LogTemp, Error, TEXT("Weapon BeginPlay: BulletStorage NOT FOUND"));
    }

    Tags.AddUnique(FName("Weapon"));
}


void AEnemy_Weapon_1::BulletFire(float Time ,AActor* Caller)
{
    if (Caller != GetOwner())
    {
        return;
    }

    if (!m_bulletPool)
    {
        return;
    }
    AEnemy_Bullet_1* Bullet = m_bulletPool->GetBullet();
    if (!Bullet)
    {
        return;
    }
    float IntervalTime = 0.;

    //DeltaTime代わり（このクラスにはDeltaTimeがないため）
    IntervalTime = Time - m_recordTime;

    m_shotTime += IntervalTime;

    //弾がなくなったらいロード
    if (m_maxAmo <= m_usedAmo)
    {
        //リロード時間カウント
        m_reloadTime += IntervalTime;
        //リロードが完了したら
        if (m_reloadTime > m_reloadTime_Limit)
        {
            m_usedAmo = 0;
            m_reloadTime = 0;
        }
    }
    //弾の発射タイミング
    else if (m_shotTime>m_shotTime_Limit)
    {
        FVector StartPos = GetActorLocation();
        FVector StartDir = GetActorForwardVector();

        Bullet->ActivateBullet(StartPos, StartDir);
        //弾の消費
        m_usedAmo += 1;
        //発射タイミングリセット
        m_shotTime = 0;
    }

    //時間の更新
    m_recordTime = Time;
}