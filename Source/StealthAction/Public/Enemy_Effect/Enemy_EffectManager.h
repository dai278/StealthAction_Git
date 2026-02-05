// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_EffectManager.generated.h"

class AEnemy_Effect_1;

UCLASS()
class STEALTHACTION_API AEnemy_EffectManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemy_EffectManager();

	AEnemy_Effect_1* GetEffect1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere, Category = "Effect")
	TSubclassOf<AEnemy_Effect_1> Effect1Class;

	UPROPERTY()
	TArray<AEnemy_Effect_1*> Effect1Pool;

	UPROPERTY(EditAnywhere, Category = "Effect")
	int m_effect1Stock;

};
