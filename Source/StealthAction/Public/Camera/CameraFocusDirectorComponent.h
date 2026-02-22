// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraFocusProvider.h"

#include "CameraFocusDirectorComponent.generated.h"

class APlayerController;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHACTION_API UCameraFocusDirectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraFocusDirectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every fra
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
    // 開始処理
    //UFUNCTION(BlueprintCallable, Category = "Camera")
    void StartFocusByProviderActor(AActor* ProviderActor,bool& isStart,APlayerController* _controller);

    void StartFocus(AActor* ProviderActor, bool& isStart, APlayerController* _controller);


    // 終了処理
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void ForceRestore();

private:
    void RestoreInternal(bool bUnpause);

	//時間での復帰処理
	void RestoreByTime();

	//手動での復帰処理
	void RestoreByInput();

private:
    TWeakObjectPtr<AActor> SavedViewTarget;
    bool bFocusing = false;
    bool bPausedGame = false;

    float SavedBlendOutTime = 0.3f;
    double EndRealTime = 0.0;

	ERewindTriggerType m_rewindTriggerType = ERewindTriggerType::None;
};
