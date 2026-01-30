// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/PlayerController.h"
//#include "MyPlayerController.generated.h"
//
////前方宣言
//class URadarWidget;
//class UHUDWidget;
//class APlayerCharacter;
//class UShadowEffect;
//
//enum class EPlayerStatus : uint8;
//
//UCLASS(Blueprintable, BlueprintType)
//class STEALTHACTION_API AMyPlayerController : public APlayerController
//{
//	GENERATED_BODY()
//
//protected:
//	//ゲーム開始時
//    virtual void BeginPlay() override;
//
//	//毎フレーム更新
//    virtual void Tick(float DeltaSeconds) override;
//
//
//protected:
//
//	// ============================
//	// HUD全体（影ゲージを含む）
//	// ============================
//	UPROPERTY()
//	UHUDWidget* HUDWidget;
//
//	// HUDのクラス（BPで指定）
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
//	TSubclassOf<UHUDWidget> HUDWidgetClass;
//
//	// ============================
//	// レーダーUI
//	// ============================
//	UPROPERTY()
//	URadarWidget* RadarWidget;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
//	TSubclassOf<URadarWidget> RadarWidgetClass;
//
//	//=============================
//	//影UI
//	//=============================
//	UPROPERTY()
//	UShadowEffect* m_pShadowEffect;
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
//	TSubclassOf<UShadowEffect> ShadowEffect;
//
//	// ============================
//	// プレイヤー参照
//	// ============================
//	UPROPERTY()
//	APlayerCharacter* PlayerCharacter;
//
//};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "MyPlayerController.generated.h"

// ===== 前方宣言 =====
class URadarWidget;
class UHUDWidget;
class APlayerCharacter;
class UShadowEffect;

// 入力モード種別
UENUM(BlueprintType)
enum class EInputModeType : uint8
{
	Player,
	Menu
};

UCLASS(BlueprintType)
class STEALTHACTION_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

protected:
	// ゲーム開始時
	virtual void BeginPlay() override;

	// 毎フレーム更新
	virtual void Tick(float DeltaSeconds) override;

public:
	// ===== IMC 切り替え用 =====
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetInputMode_Player();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetInputMode_Menu();

protected:
	void ApplyIMC(EInputModeType NewMode);

	// ============================
	// Input Mapping Context（★BPで設定する）
	// ============================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* PlayerIMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MenuIMC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	EInputModeType CurrentInputMode = EInputModeType::Menu;

	// ============================
	// HUD
	// ============================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UHUDWidget* HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UHUDWidget> HUDWidgetClass;

	// ============================
	// レーダーUI
	// ============================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	URadarWidget* RadarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<URadarWidget> RadarWidgetClass;

	// ============================
	// 影UI
	// ============================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UShadowEffect* ShadowEffectWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UShadowEffect> ShadowEffectClass;

	// ============================
	// プレイヤー参照
	// ============================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	APlayerCharacter* PlayerCharacter;
};
