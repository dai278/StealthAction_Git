// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/Interact.h"
#include "Components/BoxComponent.h"

// Sets default values
AInteract::AInteract()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	//メッシュコンポーネント生成
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(m_pMesh);
	//コリジョンコンポーネント生成
	m_pCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	m_pCollision->SetupAttachment(m_pMesh);

	Tags.Add(TEXT("Interact"));


}

// Called when the game starts or when spawned
void AInteract::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteract::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

