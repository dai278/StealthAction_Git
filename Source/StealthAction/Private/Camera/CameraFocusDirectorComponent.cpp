//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraFocusDirectorComponent.h"
#include "Camera/CameraActor.h"


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
// –ˆƒtƒŒ[ƒ€‚Ìˆ—
//	----------------------------------------------
void UCameraFocusDirectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bFocusing) return;

	switch (m_rewindTriggerType)
	{
	case ERewindTriggerType::TimedAuto:
		RestoreByTime();
		break;
	case ERewindTriggerType::Manual:
		RestoreByInput();
		break;
	default:
		break;
	}
}


//----------------------------------------------
// ŠJnˆ—
//----------------------------------------------
void UCameraFocusDirectorComponent::StartFocusByProviderActor(AActor* ProviderActor, bool& isStart, APlayerController* _controller)
{
	//nullƒ`ƒFƒbƒN
	if (!IsValid(ProviderActor) || !IsValid(_controller))
	{
		isStart = false;
		return;
	}

	//ƒCƒ“ƒ^[ƒtƒF[ƒXÀ‘•ƒ`ƒFƒbƒN
	if (!ProviderActor->GetClass()->ImplementsInterface(UCameraFocusProvider::StaticClass()))
	{
		isStart = false;
		return;
	}

	// ƒf[ƒ^æ“¾
	const FCameraFocusData Data = ICameraFocusProvider::Execute_GetCameraFocusData(ProviderActor);
	// ƒJƒƒ‰‚Ì—L–³ƒ`ƒFƒbƒN
	if (!IsValid(Data.FocusCamera) || !IsValid(_controller))
	{
		isStart = false;
		return;
	}
	
	//APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
  //  if (!_controller)
  //  {
		//isStart = false;
  //      return;
  //  }
	// ‘½dŠJn‚Ì–\”­–h~F‚Ü‚¸•œ‹Aiƒ|[ƒY‰ğœŠÜ‚Şj‚µ‚Ä‚©‚çã‘‚«
	if (bFocusing)
	{
		RestoreInternal(/*bUnpause*/true);
	}
	
	// Šª‚«–ß‚éÛ‚ÌƒgƒŠƒK[ƒ^ƒCƒv‚ğ•Û‘¶
	m_rewindTriggerType = Data.RewindTriggerType;

	//Œ»İ‚ÌViewTarget‚ğ•Û‘¶
	SavedViewTarget = _controller->GetViewTarget();
	// ƒuƒŒƒ“ƒhƒAƒEƒgŠÔ‚ğ•Û‘¶
	SavedBlendOutTime = Data.BlendOutTime;
	// ƒ|[ƒY‚·‚é‚©H
	bPausedGame = Data.bPause;

	// Ø‘Ö
	_controller->SetViewTargetWithBlend((AActor*)Data.FocusCamera, Data.BlendTime);

	// ÀŠÔ‚ÅI—¹—\’è‚ğŒˆ‚ß‚é
	bFocusing = true;
	EndRealTime = FPlatformTime::Seconds() + ((Data.PauseTime < 0.f) ? 0 : Data.PauseTime);

	isStart = true;

	if (bPausedGame)
	{
		//ƒ|[ƒY
		UGameplayStatics::SetGamePaused(this, true);
	}


}

//----------------------------------------------
// ŠJnˆ—iProviderActor‚©‚ç‚Ìƒf[ƒ^æ“¾‚È‚µAÅ’áŒÀ‚Ìˆø”‚ÅŠJn‚·‚éê‡j
// -----------------------------------------------
void UCameraFocusDirectorComponent::StartFocus(AActor* ProviderActor, bool& isStart, APlayerController* _controller)
{

	if (!IsValid(ProviderActor) || !IsValid(_controller))
	{
		isStart = false;
		return;
	}



	FCameraFocusData Data = {
		// ƒMƒ~ƒbƒN‚²‚Æ‚É’u‚­ƒJƒƒ‰
		Cast<ACameraActor>(ProviderActor)
		, 3.f
		, 0.3f
		,true
		,false
		,true
		, 5.f
	};


	if (!IsValid(Data.FocusCamera) || !IsValid(_controller))
	{
		isStart = false;
		return;
	}

	//APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
  //  if (!_controller)
  //  {
		//isStart = false;
  //      return;
  //  }
	// ‘½dŠJn‚Ì–\”­–h~F‚Ü‚¸•œ‹Aiƒ|[ƒY‰ğœŠÜ‚Şj‚µ‚Ä‚©‚çã‘‚«
	if (bFocusing)
	{
		RestoreInternal(/*bUnpause*/true);
	}

	//Œ»İ‚ÌViewTarget‚ğ•Û‘¶
	SavedViewTarget = _controller->GetViewTarget();
	// ƒuƒŒƒ“ƒhƒAƒEƒgŠÔ‚ğ•Û‘¶
	SavedBlendOutTime = Data.BlendOutTime;
	// ƒ|[ƒY‚·‚é‚©H
	bPausedGame = Data.bPause;

	// Ø‘Ö
	_controller->SetViewTargetWithBlend((AActor*)Data.FocusCamera, Data.BlendTime);

	// ÀŠÔ‚ÅI—¹—\’è‚ğŒˆ‚ß‚é
	bFocusing = true;
	EndRealTime = FPlatformTime::Seconds() + ((Data.PauseTime < 0.f) ? 0 : Data.PauseTime);

	isStart = true;

	if (bPausedGame)
	{
		//ƒ|[ƒY
		UGameplayStatics::SetGamePaused(this, true);
	}


}


//----------------------------------------------
// ‹­§•œ‹Aˆ—
//----------------------------------------------
void UCameraFocusDirectorComponent::ForceRestore()
{
	if (!bFocusing) return;
	RestoreInternal(true);
}


//----------------------------------------------
//•œ‹Aˆ—
//----------------------------------------------
void UCameraFocusDirectorComponent::RestoreInternal(bool bUnpause)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (IsValid(PC))
	{
		AActor* ReturnTarget = SavedViewTarget.Get();
		if (IsValid(ReturnTarget))
		{
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(PC->GetPawn()))
			{
				Player->EndCameraFocus(SavedBlendOutTime);
			}
			else
			{
				PC->SetViewTargetWithBlend(ReturnTarget, SavedBlendOutTime);
			}
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

//----------------------------------------------
// ŠÔŒo‰ß‚Å•œ‹A‚·‚éê‡‚Ìˆ—
//----------------------------------------------
void UCameraFocusDirectorComponent::RestoreByTime()
{
	if (FPlatformTime::Seconds() >= EndRealTime)
	{
		RestoreInternal(/*bUnpause*/true);
	}
}

//----------------------------------------------
// “ü—Í‚Å•œ‹A‚·‚éê‡‚Ìˆ—
//----------------------------------------------
void UCameraFocusDirectorComponent::RestoreByInput()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC)) return;
	if (PC->WasInputKeyJustPressed(EKeys::E))
	{
		RestoreInternal(/*bUnpause*/true);
	}
}