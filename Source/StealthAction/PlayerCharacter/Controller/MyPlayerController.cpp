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
//	}
//
//	if (ShadowEffect) {
//		//影エフェクト生成
//		m_pShadowEffect = CreateWidget<UShadowEffect>(this, ShadowEffect);
//		m_pShadowEffect->AddToViewport();
//		m_pShadowEffect->SetVisibility(ESlateVisibility::Hidden);
//
//	}
//	//============================
//	// レーダーUI生成（既存処理）
//	//============================
//	
//    // レーダーUIのクラスが設定されていなければ何もしない
//    if (!RadarWidgetClass)
//    {
//        return;
//    }
//
//    // レーダーUI生成
//    RadarWidget = CreateWidget<URadarWidget>(this, RadarWidgetClass);
//    if (!RadarWidget)
//    {
//        return;
//    }
//
//    // 画面に追加
//    RadarWidget->AddToViewport();
//
//
//
//}
//
//void AMyPlayerController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//
//}

#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h" //自身のヘッダーを最初にincludeする

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
	if (!GetWorld()) { return; }

	ApplyIMC(EInputModeType::Player);


	// ============================
	// PlayerCharacter 取得
	// ============================
	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!PlayerCharacter)
	{
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


	// ポーズ中もTick可能にする設定
	// 自作PlayerControllerのBeginPlay等で
	bShouldPerformFullTickWhenPaused = true;

	// 念のため：PC自体もポーズ中Tick可能に
	SetTickableWhenPaused(true);

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

// Get HUDWidget
UHUDWidget* AMyPlayerController::GetHUDWidget()
{
	return IsValid(HUDWidget) ? HUDWidget : nullptr;
}

// Set HUDWidget
void AMyPlayerController::SetHUDWidget(UHUDWidget* NewHUDWidget)
{
	HUDWidget = NewHUDWidget;
}

// ============================
// IMC 適用処理
// ============================
void AMyPlayerController::ApplyIMC(EInputModeType NewMode)
{
	if (!IsValid(GetLocalPlayer())) { return; }


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
