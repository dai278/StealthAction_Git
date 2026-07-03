// Fill out your copyright notice in the Description page of Project Settings.

#include "Interact/PushMoveReset.h"
#include "Interact/PushMoveActor.h"



//--------------------------
//コンストラクタ
//--------------------------
APushMoveReset::APushMoveReset()
	:AInteract()
{
	PrimaryActorTick.bCanEverTick = true;

}

//--------------------------
//BeginPlay
//--------------------------
void APushMoveReset::BeginPlay()
{
	Super::BeginPlay();
	
}


//--------------------------
//Tick
//--------------------------
void APushMoveReset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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