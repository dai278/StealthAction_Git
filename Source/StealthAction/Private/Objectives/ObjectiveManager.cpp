// Fill out your copyright notice in the Description page of Project Settings.


#include "Objectives/ObjectiveManager.h"
#include "Objectives/ObjectivePoint.h"
#include "Objectives/ObjectiveMarkerWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

AObjectiveManager::AObjectiveManager()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentObjective = nullptr;

    bCanShowMarker = false;
}

void AObjectiveManager::BeginPlay()
{
    Super::BeginPlay();

    // Widgetê∂ê¨
    if (MarkerWidgetClass)
    {
        MarkerWidget =
            CreateWidget<UObjectiveMarkerWidget>(
                GetWorld(),
                MarkerWidgetClass);

        if (MarkerWidget)
        {
            MarkerWidget->AddToViewport();

            MarkerWidget->SetVisibility(
                ESlateVisibility::Hidden);
        }
    }

    // ç≈èâÇÃObjectiveê›íË
    if (Objectives.Num() > 0)
    {
        SetCurrentObjective(Objectives[0]);
    }
}

void AObjectiveManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!CurrentObjective ||
        !MarkerWidget ||
        !bCanShowMarker)
    {
        return;
    }

    APlayerController* PC =
        UGameplayStatics::GetPlayerController(this, 0);

    APawn* PlayerPawn =
        UGameplayStatics::GetPlayerPawn(this, 0);

    if (!PC || !PlayerPawn)
    {
        return;
    }

    FVector WorldLocation =
        CurrentObjective->GetWidgetWorldLocation();

    FVector2D ScreenPosition;

    bool bOnScreen =
        UGameplayStatics::ProjectWorldToScreen(
            PC,
            WorldLocation,
            ScreenPosition);

    if (bOnScreen)
    {
        MarkerWidget->SetVisibility(
            ESlateVisibility::Visible);

        MarkerWidget->SetPositionInViewport(
            ScreenPosition);

        float Distance =
            FVector::Distance(
                PlayerPawn->GetActorLocation(),
                WorldLocation);

        float MinDistance = 200.0f;
        float MaxDistance = 2000.0f;

        float MinScale = 0.5f;
        float MaxScale = 1.5f;

        float Alpha =
            FMath::Clamp(
                (Distance - MinDistance) /
                (MaxDistance - MinDistance),
                0.0f,
                1.0f);

        float Scale =
            FMath::Lerp(
                MaxScale,
                MinScale,
                Alpha);

        MarkerWidget->SetRenderScale(
            FVector2D(Scale, Scale));
    }
    else
    {
        MarkerWidget->SetVisibility(
            ESlateVisibility::Hidden);
    }
}

void AObjectiveManager::SetCurrentObjective(
    AObjectivePoint* NewObjective)
{
    if (CurrentObjective)
    {
        CurrentObjective->bIsActive = false;
    }

    CurrentObjective = NewObjective;

    if (!CurrentObjective)
    {
        HideMarker();
        return;
    }

    CurrentObjective->bIsActive = true;

    HideMarker();

    StartMarkerTimer();
}

void AObjectiveManager::StartMarkerTimer()
{
    bCanShowMarker = false;

    GetWorld()->GetTimerManager().ClearTimer(
        MarkerTimerHandle);

    GetWorld()->GetTimerManager().SetTimer(
        MarkerTimerHandle,
        this,
        &AObjectiveManager::ShowMarker,
        MarkerDelayTime,
        false);
}

void AObjectiveManager::ShowMarker()
{
    bCanShowMarker = true;
}

void AObjectiveManager::HideMarker()
{
    bCanShowMarker = false;

    GetWorld()->GetTimerManager().ClearTimer(
        MarkerTimerHandle);

    if (MarkerWidget)
    {
        MarkerWidget->SetVisibility(
            ESlateVisibility::Hidden);
    }
}