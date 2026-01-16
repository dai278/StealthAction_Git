// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

//前方宣言
class URadarWidget;
class UHUDWidget;
class APlayerCharacter;

UCLASS(Blueprintable, BlueprintType)
class STEALTHACTION_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	//ゲーム開始時
    virtual void BeginPlay() override;

	//毎フレーム更新
    virtual void Tick(float DeltaSeconds) override;

protected:

	// ============================
	// HUD全体（影ゲージを含む）
	// ============================
	UPROPERTY()
	UHUDWidget* HUDWidget;

	// HUDのクラス（BPで指定）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UHUDWidget> HUDWidgetClass;

	// ============================
	// レーダーUI
	// ============================
	UPROPERTY()
	URadarWidget* RadarWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<URadarWidget> RadarWidgetClass;

	// ============================
	// プレイヤー参照
	// ============================
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

};
