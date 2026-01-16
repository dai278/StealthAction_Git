// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h"

#include "UI/RadarMap/RadarWidget.h"
#include "UI/HUD/HUDWidget.h"

#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"


void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

	//============================
	// PlayerCharacter を取得
	//============================
	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter not found"));
	}

	//============================
	// HUDWidget を生成
	//============================
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HUDWidgetClass is not set"));
	}

	//============================
	// レーダーUI生成（既存処理）
	//============================
	
    // レーダーUIのクラスが設定されていなければ何もしない
    if (!RadarWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("RadarWidgetClass is not set"));
        return;
    }

    // レーダーUI生成
    RadarWidget = CreateWidget<URadarWidget>(this, RadarWidgetClass);
    if (!RadarWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create RadarWidget"));
        return;
    }

    // 画面に追加
    RadarWidget->AddToViewport();

    UE_LOG(LogTemp, Warning, TEXT("RadarWidget created and added to viewport"));
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}

