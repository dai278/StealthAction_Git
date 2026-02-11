// Fill out your copyright notice in the Description page of Project Settings.


#include "StageObject/OutLooKDoor.h"
#include "KeyItemSubsystem.h"
#include "ItemKey.h"

// Sets default values
AOutLooKDoor::AOutLooKDoor()
	:AActor()
	, m_closeLocation(FVector::ZeroVector)
	, m_closeSpeed(100.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_doorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	SetRootComponent(m_doorMesh);

}

// Called when the game starts or when spawned
void AOutLooKDoor::BeginPlay()
{
	Super::BeginPlay();


	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AOutLooKDoor::OnFrastTick);

	//チック無効化
	SetActorTickEnabled(false);

}

// Called every frame
void AOutLooKDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ドアを閉める処理
	FVector currentLocation = GetActorLocation();
	FVector direction = (m_closeLocation - currentLocation).GetSafeNormal();
	FVector newLocation = currentLocation + direction * m_closeSpeed * DeltaTime;
	SetActorLocation(newLocation);
	//閉まったかの判定
	if (FVector::Dist(newLocation, m_closeLocation) <= 10.f)
	{
		SetActorLocation(m_closeLocation);
		//チック無効化
		SetActorTickEnabled(false);
	}
	

}


//閉める処理
void AOutLooKDoor::CloseDoor()
{
	//チック有効化
	SetActorTickEnabled(true);
}


//チック1フレーム目にのみ呼ばれる処理
void AOutLooKDoor::OnFrastTick()
{
	UKeyItemSubsystem* KeyItemSubsystem = GetWorld()->GetSubsystem<UKeyItemSubsystem>();

	if (KeyItemSubsystem)
	{
		AItemKey* key = KeyItemSubsystem->GetKeyItem();

		key->SetOnGetKeyItemCallback(std::bind(&AOutLooKDoor::CloseDoor, this));

		//チックしない
		SetActorTickEnabled(false);

	}

}