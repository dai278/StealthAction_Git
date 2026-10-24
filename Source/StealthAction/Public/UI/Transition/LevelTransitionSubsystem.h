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
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual TStatId GetStatId() const override;

	//暗転→レベル移動→明転。実行中の追加要求は無視する
	UFUNCTION(BlueprintCallable, Category = "Level Transition|Fade")
	bool OpenLevelWithFade(FName LevelName, float FadeOutTime, float BlackHoldTime, float FadeInTime, FLinearColor Color);
	UFUNCTION(BlueprintPure, Category = "Level Transition|Fade")
	bool IsTransitionActive() const;

private:
	enum class EFadeStage : uint8 { None, FadeOut, Holding, Loading, FadeIn };
	//フェードの時間と画面を更新
	void AdvanceFade(float DeltaTime);
	void SetFadeOpacity(float Opacity);
	void FinishTransition();
	void OnMapLoaded(UWorld* LoadedWorld);
	void OnTravelFailed(UWorld* World, ETravelFailure::Type FailureType, const FString& Error);
	TSharedPtr<SBorder> FadeWidget;
	TWeakObjectPtr<UGameViewportClient> FadeViewport;
	FDelegateHandle MapLoadedHandle;
	FDelegateHandle TravelFailureHandle;
	EFadeStage Stage = EFadeStage::None;
	FName DestinationLevel = NAME_None;
	FLinearColor FadeColor = FLinearColor::Black;
	float Elapsed = 0.f;
	float FadeOutDuration = 0.f;
	float HoldDuration = 0.f;
	float FadeInDuration = 0.f;
};
