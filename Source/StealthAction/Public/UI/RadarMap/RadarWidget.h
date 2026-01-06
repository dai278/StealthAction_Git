// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadarWidget.generated.h"

class AEnemy_1;
class APlayerCharacter;

UCLASS()
class STEALTHACTION_API URadarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// コンストラクタ 
	URadarWidget(const FObjectInitializer& ObjectInitializer);

	// 所有プレイヤー設定 
	UFUNCTION(BlueprintCallable)
	void SetOwnerPlayer(APlayerCharacter* Player);

	// ===== レーダー設定 ===== 

	// // UI上のレーダー半径 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar")
	float RadarRadius = 80.f;

	// ワールド上の探知距離 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar")
	float RadarRange = 1500.f;
protected:
	// ウィジェット生成時 
	virtual void NativeConstruct() override;

	// 描画処理 
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId, const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const override;

protected:
	// レーダーの基準となるプレイヤー 
	UPROPERTY()
	APlayerCharacter* OwnerPlayer = nullptr;
	
};
