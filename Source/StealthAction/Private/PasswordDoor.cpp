// Fill out your copyright notice in the Description page of Project Settings.


#include "PasswordDoor.h"
#include "Components/StaticMeshComponent.h"

APasswordDoor::APasswordDoor()
	: m_password("")
	, m_bIsOpen(false)
{
	//Tick‚Íg‚í‚È‚¢
	PrimaryActorTick.bCanEverTick = false;

	//ƒƒbƒVƒ…‚Ìì¬
	m_pDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = m_pDoorMesh;
}
