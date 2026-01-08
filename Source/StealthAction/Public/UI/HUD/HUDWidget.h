// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class URadarWidget;
class UPlayerHPGaugeWidget;

UCLASS()
class STEALTHACTION_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Radar（BP側でバインド）
	UPROPERTY(meta = (BindWidget))
	URadarWidget* RadarWidget;

	// HPゲージ（BP側でバインド）
	UPROPERTY(meta = (BindWidget))
	UPlayerHPGaugeWidget* PlayerHPGaugeWidget;

};
