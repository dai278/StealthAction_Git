// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Effect/Enemy_EffectManager.h"
#include "Enemy_Effect/Enemy_Effect_1.h"
#include "Enemy_Effect/Enemy_Effect_2.h"
#include "Enemy_Effect/Enemy_Effect_3.h"
#include "Enemy_Effect/Enemy_Effect_4.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy_EffectManager::AEnemy_EffectManager()
    :m_effect1Stock(10)
    , m_effect2Stock(10)
    , m_effect3Stock(10)
    , m_effect4Stock(10)

{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemy_EffectManager::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World) return;

    for (int i = 0; i < m_effect1Stock; i++)
    {
        AEnemy_Effect_1* Effect = World->SpawnActor<AEnemy_Effect_1>(Effect1Class);
        if (!Effect) continue;

        Effect->InvisibleEffect();
        Effect1Pool.Add(Effect);
    }

    for (int i = 0; i < m_effect2Stock; i++)
    {
        AEnemy_Effect_2* Effect = World->SpawnActor<AEnemy_Effect_2>(Effect2Class);
        if (!Effect) continue;

        Effect->InvisibleEffect();
        Effect2Pool.Add(Effect);
    }
    for (int i = 0; i < m_effect3Stock; i++)
    {
        AEnemy_Effect_3* Effect = World->SpawnActor<AEnemy_Effect_3>(Effect3Class);
        if (!Effect) continue;

        Effect->InvisibleEffect();
        Effect3Pool.Add(Effect);
    }
    for (int i = 0; i < m_effect4Stock; i++)
    {
        AEnemy_Effect_4* Effect = World->SpawnActor<AEnemy_Effect_4>(Effect4Class);
        if (!Effect) continue;

        Effect->InvisibleEffect();
        Effect4Pool.Add(Effect);
    }

}

AEnemy_Effect_1* AEnemy_EffectManager::GetEffect1()
{
    for (int i = 0; i < Effect1Pool.Num(); i++)
    {
        AEnemy_Effect_1* Effect = Effect1Pool[i];
        if (Effect && Effect->bHidden)
        {
            return Effect;
        }
    }
    return nullptr;
}

AEnemy_Effect_2* AEnemy_EffectManager::GetEffect2()
{
    for (int i = 0; i < Effect2Pool.Num(); i++)
    {
        AEnemy_Effect_2* Effect = Effect2Pool[i];
        if (Effect && Effect->bHidden)
        {
            return Effect;
        }
    }
    return nullptr;
}

AEnemy_Effect_3* AEnemy_EffectManager::GetEffect3()
{
    for (int i = 0; i < Effect3Pool.Num(); i++)
    {
        AEnemy_Effect_3* Effect = Effect3Pool[i];
        if (Effect && Effect->bHidden)
        {
            return Effect;
        }
    }
    return nullptr;
}

AEnemy_Effect_4* AEnemy_EffectManager::GetEffect4()
{
    for (int i = 0; i < Effect4Pool.Num(); i++)
    {
        AEnemy_Effect_4* Effect = Effect4Pool[i];
        if (Effect && Effect->bHidden)
        {
            return Effect;
        }
    }
    return nullptr;
}
