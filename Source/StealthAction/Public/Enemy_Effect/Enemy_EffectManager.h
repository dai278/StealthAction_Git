// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_EffectManager.generated.h"

class AEnemy_Effect_1;
class AEnemy_Effect_2;
class AEnemy_Effect_3;
class AEnemy_Effect_4;

UCLASS()
class STEALTHACTION_API AEnemy_EffectManager : public AActor
{
	GENERATED_BODY()

public:
	AEnemy_EffectManager();

protected:
	virtual void BeginPlay() override;

public:
	AEnemy_Effect_1* GetEffect1();
	AEnemy_Effect_2* GetEffect2();
	AEnemy_Effect_3* GetEffect3();
	AEnemy_Effect_4* GetEffect4();

private:
	template<typename T>
	void PrewarmPoolTyped(TSubclassOf<T> Class, int32 Stock, TArray<T*>& Pool);

private:
	UPROPERTY(EditAnywhere, Category = "Effect|Class")
	TSubclassOf<AEnemy_Effect_1> Effect1Class;

	UPROPERTY(EditAnywhere, Category = "Effect|Class")
	TSubclassOf<AEnemy_Effect_2> Effect2Class;

	UPROPERTY(EditAnywhere, Category = "Effect|Class")
	TSubclassOf<AEnemy_Effect_3> Effect3Class;

	UPROPERTY(EditAnywhere, Category = "Effect|Class")
	TSubclassOf<AEnemy_Effect_4> Effect4Class;

	UPROPERTY(EditAnywhere, Category = "Effect|Stock")
	int32 m_effect1Stock;

	UPROPERTY(EditAnywhere, Category = "Effect|Stock")
	int32 m_effect2Stock;

	UPROPERTY(EditAnywhere, Category = "Effect|Stock")
	int32 m_effect3Stock;

	UPROPERTY(EditAnywhere, Category = "Effect|Stock")
	int32 m_effect4Stock;

	// ÉvÅ[ÉãÇÕéQè∆ï€éùÇÃÇΩÇﬂUPROPERTYêÑèß
	UPROPERTY()
	TArray<AEnemy_Effect_1*> Effect1Pool;

	UPROPERTY()
	TArray<AEnemy_Effect_2*> Effect2Pool;

	UPROPERTY()
	TArray<AEnemy_Effect_3*> Effect3Pool;

	UPROPERTY()
	TArray<AEnemy_Effect_4*> Effect4Pool;

};
