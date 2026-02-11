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

	//キーアイテムを取得しているか
	UPROPERTY(BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	bool bHasKeyItem = false;

	



public:

	// キーアイテム取得状態設定関数
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetHasKeyItem(bool bNewHasKeyItem) { bHasKeyItem = bNewHasKeyItem; };

	// キーアイテム取得状態取得関数
	UFUNCTION(BlueprintCallable, Category = "HUD")
	bool GetHasKeyItem() const { return bHasKeyItem; };

	// PlayerController から呼ばれる更新関数
	void UpdateShadowTime(float Rate);
};
