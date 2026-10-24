//----------------------------------------------------------
//概要：画面フェードとレベル移動
//----------------------------------------------------------

#include "UI/Transition/LevelTransitionSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateColor.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/Layout/SBorder.h"

//-----------------------------------------------------
//初期化
//-----------------------------------------------------
void ULevelTransitionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	MapLoadedHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULevelTransitionSubsystem::OnMapLoaded);
	if (GEngine)
	{
		TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &ULevelTransitionSubsystem::OnTravelFailed);
	}
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void ULevelTransitionSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(MapLoadedHandle);
	if (GEngine)
	{
		GEngine->OnTravelFailure().Remove(TravelFailureHandle);
	}
	FinishTransition();
	Super::Deinitialize();
}

//-----------------------------------------------------
//フェード開始
//-----------------------------------------------------
bool ULevelTransitionSubsystem::OpenLevelWithFade(FName LevelName, float FadeOutTime, float BlackHoldTime, float FadeInTime,
												  FLinearColor Color)
{
	if (IsTransitionActive())
	{
		return true;
	}
	UGameInstance* Instance = GetGameInstance();
	UGameViewportClient* Viewport = Instance ? Instance->GetGameViewportClient() : nullptr;
	if (LevelName.IsNone() || !GetTickableGameObjectWorld() || !IsValid(Viewport))
	{
		return false;
	}
	DestinationLevel = LevelName;
	FadeOutDuration = FMath::Max(0.f, FadeOutTime);
	HoldDuration = FMath::Max(0.f, BlackHoldTime);
	FadeInDuration = FMath::Max(0.f, FadeInTime);
	FadeColor = Color;
	FadeColor.A = 1.f;
	Elapsed = 0.f;
	Stage = EFadeStage::FadeOut;

	//UMGも含めて暗くするため画面の一番手前に重ねる
	FadeWidget = SNew(SBorder)
					 .BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush")))
					 .BorderBackgroundColor(FSlateColor(FLinearColor(FadeColor.R, FadeColor.G, FadeColor.B, 0.f)))
					 .Visibility(EVisibility::HitTestInvisible);
	FadeViewport = Viewport;
	Viewport->AddViewportWidgetContent(FadeWidget.ToSharedRef(), 100000);
	return true;
}

//-----------------------------------------------------
//画面遷移中か
//-----------------------------------------------------
bool ULevelTransitionSubsystem::IsTransitionActive() const
{
	return Stage != EFadeStage::None;
}

//-----------------------------------------------------
//更新するか
//-----------------------------------------------------
bool ULevelTransitionSubsystem::IsTickable() const
{
	return !IsTemplate() && IsTransitionActive();
}

//-----------------------------------------------------
//更新対象のワールド取得
//-----------------------------------------------------
UWorld* ULevelTransitionSubsystem::GetTickableGameObjectWorld() const
{
	const UGameInstance* Instance = GetGameInstance();
	return Instance ? Instance->GetWorld() : nullptr;
}

//-----------------------------------------------------
//計測用ID取得
//-----------------------------------------------------
TStatId ULevelTransitionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULevelTransitionSubsystem, STATGROUP_Tickables);
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void ULevelTransitionSubsystem::Tick(float DeltaTime)
{
	//ポーズやゲーム内の時間倍率に左右されない時間を使う
	AdvanceFade(static_cast<float>(FApp::GetDeltaTime()));
}

//-----------------------------------------------------
//暗転が終わってから1回だけレベル移動
//-----------------------------------------------------
void ULevelTransitionSubsystem::AdvanceFade(float DeltaTime)
{
	Elapsed += FMath::Max(0.f, DeltaTime);
	switch (Stage)
	{
	case EFadeStage::FadeOut:
		SetFadeOpacity(FadeOutDuration > 0.f ? FMath::Clamp(Elapsed / FadeOutDuration, 0.f, 1.f) : 1.f);
		if (Elapsed >= FadeOutDuration)
		{
			Stage = EFadeStage::Holding;
			Elapsed = 0.f;
		}
		break;
	case EFadeStage::Holding:
		if (Elapsed >= HoldDuration)
		{
			Stage = EFadeStage::Loading;
			Elapsed = 0.f;
			UGameplayStatics::OpenLevel(GetGameInstance(), DestinationLevel);
		}
		break;
	case EFadeStage::FadeIn:
		SetFadeOpacity(FadeInDuration > 0.f ? 1.f - FMath::Clamp(Elapsed / FadeInDuration, 0.f, 1.f) : 0.f);
		if (Elapsed >= FadeInDuration)
		{
			FinishTransition();
		}
		break;
	default:
		break;
	}
}

//-----------------------------------------------------
//レベルのロード完了後に明るく戻す
//-----------------------------------------------------
void ULevelTransitionSubsystem::OnMapLoaded(UWorld* LoadedWorld)
{
	if (!IsTransitionActive() || !LoadedWorld || LoadedWorld->GetGameInstance() != GetGameInstance())
	{
		return;
	}
	if (Stage != EFadeStage::Loading)
	{
		FinishTransition();
		return;
	}

	//ロード側で画面のウィジェットが整理されても最前面へ戻す
	UGameViewportClient* Viewport = GetGameInstance()->GetGameViewportClient();
	if (IsValid(Viewport) && FadeWidget.IsValid())
	{
		if (UGameViewportClient* PreviousViewport = FadeViewport.Get())
		{
			PreviousViewport->RemoveViewportWidgetContent(FadeWidget.ToSharedRef());
		}
		Viewport->AddViewportWidgetContent(FadeWidget.ToSharedRef(), 100000);
		FadeViewport = Viewport;
	}
	Stage = EFadeStage::FadeIn;
	Elapsed = 0.f;
	SetFadeOpacity(1.f);
	if (FadeInDuration <= 0.f)
	{
		FinishTransition();
	}
}

//-----------------------------------------------------
//移動失敗時に暗転が残り続けないようにする
//-----------------------------------------------------
void ULevelTransitionSubsystem::OnTravelFailed(UWorld* World, ETravelFailure::Type FailureType, const FString& Error)
{
	if (!IsTransitionActive() || (World && World->GetGameInstance() != GetGameInstance()))
	{
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Level transition failed: %s"), *Error);
	FinishTransition();
}

//-----------------------------------------------------
//画面の不透明度変更
//-----------------------------------------------------
void ULevelTransitionSubsystem::SetFadeOpacity(float Opacity)
{
	if (FadeWidget.IsValid())
	{
		FadeWidget->SetBorderBackgroundColor(
			FSlateColor(FLinearColor(FadeColor.R, FadeColor.G, FadeColor.B, FMath::Clamp(Opacity, 0.f, 1.f))));
	}
}

//-----------------------------------------------------
//フェード用画面の後片付け
//-----------------------------------------------------
void ULevelTransitionSubsystem::FinishTransition()
{
	if (UGameViewportClient* Viewport = FadeViewport.Get())
	{
		if (FadeWidget.IsValid())
		{
			Viewport->RemoveViewportWidgetContent(FadeWidget.ToSharedRef());
		}
	}
	FadeWidget.Reset();
	FadeViewport.Reset();
	Stage = EFadeStage::None;
	Elapsed = 0.f;
}
