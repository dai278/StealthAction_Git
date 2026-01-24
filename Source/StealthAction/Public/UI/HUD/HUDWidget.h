// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class URadarWidget;
class UPlayerHPGaugeWidget;
class UShadowTimeWidget;

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

	//影滞在時間UI(BP側でバインド)
	UPROPERTY(meta = (BindWidget))
	UShadowTimeWidget* ShadowTimeWidget;




public:
	// PlayerController から呼ばれる更新関数
	void UpdateShadowTime(float Rate);
};
