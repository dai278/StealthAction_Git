// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_Effect_1.generated.h"

UCLASS()
class STEALTHACTION_API AEnemy_Effect_1 : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemy_Effect_1();

	void ActivateEffect(const FVector& StartPos, AActor* TargetActor);
	void InvisibleEffect();
	bool IsHidden() const { return bHidden; }

	TArray<AEnemy_Effect_1*> m_pEnemy_Effect_1;

private:

	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* Billboard;
public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "EffectTime")
	double m_effectDeleteTime;			//è¡ñ≈éûä‘

	FTimerHandle DestroyTimer;
	bool bHidden = true;

};
