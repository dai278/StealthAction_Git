// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_BulletStorage_1.generated.h"

class AEnemy_Bullet_1;

UCLASS()
class STEALTHACTION_API AEnemy_BulletStorage_1 : public AActor
{
    GENERATED_BODY()

public:
    AEnemy_BulletStorage_1();

    virtual void BeginPlay() override;

    AEnemy_Bullet_1* GetBullet();


private:
    UPROPERTY(EditAnywhere, Category = "Bullet")
    TSubclassOf<AEnemy_Bullet_1> BulletClass;

    UPROPERTY()
    TArray<AEnemy_Bullet_1*> BulletPool;

    UPROPERTY(EditAnywhere, Category = "Bullet")
    int BulletStock;

};