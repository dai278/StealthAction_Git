// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h"

#include "UI/RadarMap/RadarWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

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

    // 今回は Tick では特に処理なし

}