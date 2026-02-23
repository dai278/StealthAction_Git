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

    // マーカーWidgetクラス（BP指定）
    UPROPERTY(EditAnywhere)
    TSubclassOf<UObjectiveMarkerWidget> MarkerWidgetClass;

    UPROPERTY()
    UObjectiveMarkerWidget* MarkerWidget;

    UPROPERTY(EditAnywhere)
    TArray<AObjectivePoint*> Objectives;

private:
    AObjectivePoint* CurrentObjective;

public:

    void SetCurrentObjective(AObjectivePoint* NewObjective);
};
