// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UHUDWidget;

UCLASS()
class STEALTHACTION_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
	// HUD（InGame UI 全体）
	UPROPERTY()
	UHUDWidget* HUDWidget;

	// HUD Widget クラス（BP指定）
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UHUDWidget> HUDWidgetClass;
};
