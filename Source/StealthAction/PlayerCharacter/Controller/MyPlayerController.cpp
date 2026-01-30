// Fill out your copyright notice in the Description page of Project Settings.


//#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h"
//#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
//
//#include "UI/RadarMap/RadarWidget.h"
//#include "UI/HUD/HUDWidget.h"
//
//#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
//#include "Blueprint/UserWidget.h"
//
//#include "Shadow/ShadowEffect.h"
//
//
//
//void AMyPlayerController::BeginPlay()
//{
//    Super::BeginPlay();
//
//	//============================
//	// PlayerCharacter を取得
//	//============================
//	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
//
//	if (!PlayerCharacter)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter not found"));
//	}
//
//
//	//============================
//	// HUDWidget を生成
//	//============================
//	if (HUDWidgetClass)
//	{
//		HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
//		if (HUDWidget)
//		{
//			HUDWidget->AddToViewport();
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("HUDWidgetClass is not set"));
//	}
//
//	if (ShadowEffect) {
//		//影エフェクト生成
//		m_pShadowEffect = CreateWidget<UShadowEffect>(this, ShadowEffect);
//		m_pShadowEffect->AddToViewport();
//		m_pShadowEffect->SetVisibility(ESlateVisibility::Hidden);
//		UE_LOG(LogTemp, Display, TEXT("seisei"));
//
//	}
//	//============================
//	// レーダーUI生成（既存処理）
//	//============================
//	
//    // レーダーUIのクラスが設定されていなければ何もしない
//    if (!RadarWidgetClass)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("RadarWidgetClass is not set"));
//        return;
//    }
//
//    // レーダーUI生成
//    RadarWidget = CreateWidget<URadarWidget>(this, RadarWidgetClass);
//    if (!RadarWidget)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("Failed to create RadarWidget"));
//        return;
//    }
//
//    // 画面に追加
//    RadarWidget->AddToViewport();
//
//
//
//    UE_LOG(LogTemp, Warning, TEXT("RadarWidget created and added to viewport"));
//}
//
//void AMyPlayerController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//
//}

#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h" // ★必ず最上段

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

#include "UI/HUD/HUDWidget.h"
#include "UI/RadarMap/RadarWidget.h"
#include "Shadow/ShadowEffect.h"

AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = false;
}


void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ApplyIMC(EInputModeType::Player);


	// ============================
	// PlayerCharacter 取得
	// ============================
	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter not found"));
	}

	// ============================
	// HUD 生成
	// ============================
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	// ============================
	// 影エフェクト UI
	// ============================
	if (ShadowEffectClass)
	{
		ShadowEffectWidget = CreateWidget<UShadowEffect>(this, ShadowEffectClass);
		if (ShadowEffectWidget)
		{
			ShadowEffectWidget->AddToViewport();
			ShadowEffectWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// ============================
	// レーダーUI
	// ============================
	if (RadarWidgetClass)
	{
		RadarWidget = CreateWidget<URadarWidget>(this, RadarWidgetClass);
		if (RadarWidget)
		{
			RadarWidget->AddToViewport();
		}
	}

	// ============================
	// レベル名で InputMode 判定
	// ============================
	const FString LevelName = GetWorld()->GetMapName();

	if (LevelName.Contains(TEXT("Title")) ||
		LevelName.Contains(TEXT("GameOver")) ||
		LevelName.Contains(TEXT("GameClear")))
	{
		SetInputMode_Menu();
	}
	else
	{
		SetInputMode_Player();
	}

	UE_LOG(LogTemp, Warning, TEXT("PlayerIMC: %s"), PlayerIMC ? TEXT("OK") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("MenuIMC: %s"), MenuIMC ? TEXT("OK") : TEXT("NULL"));

}

void AMyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// ============================
// IMC : Player
// ============================
void AMyPlayerController::SetInputMode_Player()
{
	ApplyIMC(EInputModeType::Player);

	FInputModeGameOnly Mode;
	SetInputMode(Mode);

	bShowMouseCursor = false;
}

// ============================
// IMC : Menu
// ============================
void AMyPlayerController::SetInputMode_Menu()
{
	ApplyIMC(EInputModeType::Menu);

	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);

	bShowMouseCursor = true;
}

// ============================
// IMC 適用処理
// ============================
void AMyPlayerController::ApplyIMC(EInputModeType NewMode)
{
	UE_LOG(LogTemp, Warning, TEXT("ApplyIMC: %d"), (int32)NewMode);

	if (CurrentInputMode == NewMode) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();

		if (NewMode == EInputModeType::Player && PlayerIMC)
		{
			Subsystem->AddMappingContext(PlayerIMC, 0);
		}
		else if (NewMode == EInputModeType::Menu && MenuIMC)
		{
			Subsystem->AddMappingContext(MenuIMC, 0);
		}
	}

	CurrentInputMode = NewMode;
}
