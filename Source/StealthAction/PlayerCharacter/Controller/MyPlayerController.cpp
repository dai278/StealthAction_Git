// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/HUDWidget.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // ç°âÒÇÕ Tick Ç≈ÇÕì¡Ç…èàóùÇ»Çµ

}