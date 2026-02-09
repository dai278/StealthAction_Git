// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraFocusDirectorComponent.h"


#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

// Sets default values for this component's properties
UCameraFocusDirectorComponent::UCameraFocusDirectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCameraFocusDirectorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


//----------------------------------------------
// 開始処理
//----------------------------------------------
void UCameraFocusDirectorComponent::StartFocusByProviderActor(AActor* ProviderActor,  bool& isStart)
{
    if (!ProviderActor)
    {
		isStart = false;
        return ;
    }

    if (!ProviderActor->GetClass()->ImplementsInterface(UCameraFocusProvider::StaticClass()))
    {
        isStart = false;
        return;
    }

    const FCameraFocusData Data = ICameraFocusProvider::Execute_GetCameraFocusData(ProviderActor);

    if (!Data.FocusCamera)
    {
        isStart = false;
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
		isStart = false;
        return;
    }
    // 多重開始の暴発防止：まず復帰（ポーズ解除含む）してから上書き
    if (bFocusing)
    {
        RestoreInternal(/*bUnpause*/true);
    }

	//現在のViewTargetを保存
    SavedViewTarget = PC->GetViewTarget();
	// ブレンドアウト時間を保存
    SavedBlendOutTime = Data.BlendOutTime;
	// ポーズするか？
    bPausedGame = Data.bPause;

    // 切替
    PC->SetViewTargetWithBlend((AActor*)Data.FocusCamera, Data.BlendTime);

    // 実時間で終了予定を決める
    bFocusing = true;
    EndRealTime = FPlatformTime::Seconds() + ((Data.PauseTime < 0.f) ? 0 : Data.PauseTime);

    isStart = true;

    if (bPausedGame)
    {
        //ポーズ
        UGameplayStatics::SetGamePaused(this, true);
    }


}

void UCameraFocusDirectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bFocusing) return;

    if (FPlatformTime::Seconds() >= EndRealTime)
    {
        RestoreInternal(/*bUnpause*/true);
    }
}


//----------------------------------------------
// 強制復帰処理
//----------------------------------------------
void UCameraFocusDirectorComponent::ForceRestore()
{
    if (!bFocusing) return;
    RestoreInternal(true);
}


//----------------------------------------------
//復帰処理
//----------------------------------------------
void UCameraFocusDirectorComponent::RestoreInternal(bool bUnpause)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        AActor* ReturnTarget = SavedViewTarget.Get();
        if (ReturnTarget)
        {
            APlayerCharacter* player =(APlayerCharacter*) PC->GetPawn();
            player->EndCameraFocus(SavedBlendOutTime);
        }
    }

    if (bUnpause && bPausedGame)
    {
        UGameplayStatics::SetGamePaused(this, false);
    }

    bFocusing = false;
    bPausedGame = false;
    SavedViewTarget.Reset();
}