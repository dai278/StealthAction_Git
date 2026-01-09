// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class URadarWidget;

UCLASS()
class STEALTHACTION_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    // ÉåÅ[É_Å[UI
    UPROPERTY()
    URadarWidget* RadarWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<URadarWidget> RadarWidgetClass;
	
};
