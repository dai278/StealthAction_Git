//----------------------------------------------------------
//概要：画面フェードとレベル移動
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Engine/EngineBaseTypes.h"
#include "LevelTransitionSubsystem.generated.h"

class SBorder;
class UGameViewportClient;

//レベルを切り替えてもフェードを維持する
UCLASS()

class STEALTHACTION_API ULevelTransitionSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

  public:

	//初期化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	//終了処理
	virtual void Deinitialize() override;

	//更新処理
	virtual void Tick(float DeltaTime) override;

	//更新するか
	virtual bool IsTickable() const override;

	//ポーズ中も更新するか
	virtual bool IsTickableWhenPaused() const override { return true; }

	//更新対象のワールド取得
	virtual UWorld* GetTickableGameObjectWorld() const override;

	//計測用ID取得
	virtual TStatId GetStatId() const override;

	//暗転してレベル移動(遷移中の追加要求は無視する)
	UFUNCTION(BlueprintCallable, Category = "Level Transition|Fade")
	bool OpenLevelWithFade(FName LevelName, float FadeOutTime, float BlackHoldTime, float FadeInTime, FLinearColor Color);

	//画面遷移中か
	UFUNCTION(BlueprintPure, Category = "Level Transition|Fade")
	bool IsTransitionActive() const;

  private:

	enum class EFadeStage : uint8
	{
		None,
		FadeOut,
		Holding,
		Loading,
		FadeIn
	};

	//フェードの時間と画面を更新
	void AdvanceFade(float DeltaTime);

	//画面の不透明度変更
	void SetFadeOpacity(float Opacity);

	//フェード終了処理
	void FinishTransition();

	//レベルのロード完了通知
	void OnMapLoaded(UWorld* LoadedWorld);

	//レベル移動失敗時の処理
	void OnTravelFailed(UWorld* World, ETravelFailure::Type FailureType, const FString& Error);

	//画面を覆うフェード用ウィジェット
	TSharedPtr<SBorder> FadeWidget;

	//フェードを表示したViewport
	TWeakObjectPtr<UGameViewportClient> FadeViewport;

	//ロード完了通知の登録情報
	FDelegateHandle MapLoadedHandle;

	//移動失敗通知の登録情報
	FDelegateHandle TravelFailureHandle;

	//現在のフェード処理
	EFadeStage Stage = EFadeStage::None;

	//移動先のレベル名
	FName DestinationLevel = NAME_None;

	//フェードの色
	FLinearColor FadeColor = FLinearColor::Black;

	//現在の処理の経過時間
	float Elapsed = 0.f;

	//暗くなるまでの時間
	float FadeOutDuration = 0.f;

	//暗転中の待ち時間
	float HoldDuration = 0.f;

	//明るく戻す時間
	float FadeInDuration = 0.f;
};
