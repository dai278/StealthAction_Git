// Fill out your copyright notice in the Description page of Project Settings.


#include "PushMoveController.h"
#include "Interact/PushMoveActor.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

// --------------------------
// コンストラクタ
// --------------------------
APushMoveController::APushMoveController()
	:AInteract()
	, m_bInteractStart(false)
{
	PrimaryActorTick.bCanEverTick = true;

	FocusProviderComp = CreateDefaultSubobject<UCameraFocusProviderComponent>(TEXT("FocusProviderComp"));

		
}


// --------------------------
// BeginPlay
// --------------------------
void APushMoveController::BeginPlay()
{
	Super::BeginPlay();
	
}

// --------------------------
// Tick
// --------------------------
void APushMoveController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// --------------------------
// Interact
// --------------------------
void APushMoveController::Interact(AActor* _interactOwner)
{
	//インタラクト開始
	m_bInteractStart = true;

	//カメラフォーカス開始
	CameraFocusStart(_interactOwner);
}


// --------------------------
// GetCameraFocusData_Implementation
// --------------------------
FCameraFocusData APushMoveController::GetCameraFocusData_Implementation() const
{
	return FocusProviderComp ? FocusProviderComp->GetCameraFocusData() : FCameraFocusData{};
}

//// --------------------------
//// CameraFocusStar
//// --------------------------
//void APushMoveController::CameraFocusStart(AActor* OtherActor)
//{
//	if (!OtherActor || !OtherActor->ActorHasTag(TEXT("Player"))) { return; }
//
//	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
//
//	FCameraFocusData FocusData = FocusProviderComp->GetCameraFocusData();
//
//	if (!FocusData.FocusCamera) { return; }
//
//	Player->StartCameraFocus(this, FocusData.BlendTime);
//
//}