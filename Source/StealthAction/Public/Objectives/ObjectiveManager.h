// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectiveManager.generated.h"

class UObjectiveMarkerWidget;
class AObjectivePoint;

UCLASS()
class STEALTHACTION_API AObjectiveManager : public AActor
{
    GENERATED_BODY()

public:

    AObjectiveManager();

protected:

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

public:

    // =========================
    // Marker Widget
    // =========================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    TSubclassOf<UObjectiveMarkerWidget> MarkerWidgetClass;

    UPROPERTY()
    UObjectiveMarkerWidget* MarkerWidget;

    // =========================
    // Objective List
    // =========================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
    TArray<AObjectivePoint*> Objectives;

    // =========================
    // 時間調整
    // =========================

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Objective|Marker",
        meta =
        (
            DisplayName = "Marker Delay Time",
            ClampMin = "0.0",
            ClampMax = "60.0",
            UIMin = "0.0",
            UIMax = "20.0"
            )
    )
    float MarkerDelayTime = 3.0f;

private:

    // =========================
    // Current Objective
    // =========================

    UPROPERTY()
    AObjectivePoint* CurrentObjective;

    // =========================
    // Timer
    // =========================

    FTimerHandle MarkerTimerHandle;

    // マーカー表示許可
    bool bCanShowMarker;

private:

    void StartMarkerTimer();

    void ShowMarker();

public:

    void SetCurrentObjective(AObjectivePoint* NewObjective);

    void HideMarker();
};