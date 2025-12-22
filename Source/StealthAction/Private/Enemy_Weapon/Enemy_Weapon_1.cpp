// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Weapon/Enemy_Weapon_1.h"
#include "Enemy_Weapon/Enemy_Bullet/Enemy_BulletStorage_1.h"
#include "Enemy_Weapon/Enemy_Bullet/Enemy_Bullet_1.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

AEnemy_Weapon_1::AEnemy_Weapon_1()
    : m_maxAmo(30)
    , m_reloadTime_Limit(3)
    , m_shotTime_Limit(0.5)
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = Mesh;

    // Mesh çÏê¨
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
}


void AEnemy_Weapon_1::BulletFire(float _deltaTime)
{
    if (!m_bulletPool)
    {
        return;
    }
    AEnemy_Bullet_1* Bullet = m_bulletPool->GetBullet();
    if (!Bullet)
    {
        return;
    }

    FVector StartPos = GetActorLocation();
    FVector StartDir = GetActorForwardVector();

    Bullet->ActivateBullet(StartPos, StartDir);


    m_shotTime += _deltaTime;

    //if (m_shotTime>m_shotTime_Limit)
    //{
    //    m_shotTime = 0;

    //}
}