// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Weapon/Enemy_Bullet/Enemy_BulletStorage_1.h"
#include "Enemy_Weapon/Enemy_Bullet/Enemy_Bullet_1.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AEnemy_BulletStorage_1::AEnemy_BulletStorage_1()
    :BulletStock(120)
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemy_BulletStorage_1::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World =GetWorld();
    if (!World) return;

    for (int i = 0; i < BulletStock; i++)
    {
        AEnemy_Bullet_1* Bullet = World->SpawnActor<AEnemy_Bullet_1>(BulletClass);
        if (!Bullet) continue;

        Bullet->InvisibleBullet();
        BulletPool.Add(Bullet);
    }
}

AEnemy_Bullet_1* AEnemy_BulletStorage_1::GetBullet()
{
    for (int i = 0; i < BulletPool.Num(); i++)
    {
        AEnemy_Bullet_1* Bullet = BulletPool[i];
        if (Bullet && Bullet->bHidden)
        {
            return Bullet;
        }
    }
    return nullptr;
}