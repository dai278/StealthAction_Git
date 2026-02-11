// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Effect/Enemy_Effect_2.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"
AEnemy_Effect_2::AEnemy_Effect_2()
	: m_effectDeleteTime(2.)

{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);

	Billboard->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Billboard->bIsScreenSizeScaled = true;

}

void AEnemy_Effect_2::ActivateEffect(const FVector& StartPos)
{

	UE_LOG(LogTemp, Warning, TEXT("EffectActive2"));

	SetActorLocation(StartPos);
	SetActorHiddenInGame(false);
	bHidden = false;

	if (Billboard)
	{
		Billboard->SetVisibility(true, true);
		Billboard->SetHiddenInGame(false);
	}
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemy_Effect_2::InvisibleEffect, m_effectDeleteTime, false);
}

void AEnemy_Effect_2::InvisibleEffect()
{
	SetActorHiddenInGame(true);
	bHidden = true;
	if (Billboard)
	{
		Billboard->SetVisibility(false, true);
		Billboard->SetHiddenInGame(true);
	}
}
