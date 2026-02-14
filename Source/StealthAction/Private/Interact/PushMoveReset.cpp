// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/PushMoveActor.h"
#include "Interact/PushMoveReset.h"


//--------------------------
//コンストラクタ
//--------------------------
APushMoveReset::APushMoveReset()
	:AInteract()
{
	PrimaryActorTick.bCanEverTick = false;
}

//--------------------------
//BeginPlay
//--------------------------
void APushMoveReset::BeginPlay()
{
	Super::BeginPlay();
	
}

//--------------------------
//Interact
//--------------------------
void APushMoveReset::Interact(AActor* _interactOwner)
{
	for (TObjectPtr<APushMoveActor> actor : m_resetActor)
	{
		if (actor)
		{
			actor->ResetMove();
		}
	}
}