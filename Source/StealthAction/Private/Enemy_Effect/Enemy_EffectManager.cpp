// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Effect/Enemy_EffectManager.h"
#include "Enemy_Effect/Enemy_Effect_1.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy_EffectManager::AEnemy_EffectManager()
    :m_effect1Stock(10)
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
