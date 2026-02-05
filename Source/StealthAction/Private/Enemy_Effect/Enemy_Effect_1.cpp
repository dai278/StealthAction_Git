// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Effect/Enemy_Effect_1.h"

AEnemy_Effect_1::AEnemy_Effect_1()
	: m_effectRadius(25.)
	, m_effectHeight(100.)
	, m_effectDeleteTime(1.)

{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// Mesh çÏê¨
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	//Mesh ÇÕìñÇΩÇËîªíËÇéùÇΩÇπÇ»Ç¢
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AEnemy_Effect_1::ActivateEffect(const FVector& StartPos, const FVector& Direction)
{
	SetActorLocation(StartPos);
	//SetActorRotation(Direction);
	SetActorHiddenInGame(false);
	bHidden = false;

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemy_Effect_1::InvisibleEffect, m_effectDeleteTime, false);
}

void AEnemy_Effect_1::InvisibleEffect()
{
	SetActorHiddenInGame(true);
	bHidden = true;
}