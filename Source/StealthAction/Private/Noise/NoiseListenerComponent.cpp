//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Noise/NoiseListenerComponent.h"
#include "Engine/World.h"
#include "Noise//NoiseManager.h"

//-----------------------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//----------------------------------------------------------
UNoiseListenerComponent::UNoiseListenerComponent()
	:m_callBackFunc(nullptr)
{
	//XV‚µ‚È‚¢
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


//---------------------------------------------------------
// BeginPlay
//----------------------------------------------------------
void UNoiseListenerComponent::BeginPlay()
{
	Super::BeginPlay();

	//ƒmƒCƒYƒ}ƒl[ƒWƒƒ[‚É“o˜^
	UNoiseManager* manager = (GetWorld() ? GetWorld()->GetSubsystem<UNoiseManager>() : nullptr);
	if (manager)
	{
		manager->AddListener(this);
	}
}


//----------------------------------------------
// Tick
//----------------------------------------------
void UNoiseListenerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//--------------------------
//ƒR[ƒ‹ƒoƒbƒNŠÖ”‚ÌŒÄ‚Ño‚µ
//--------------------------
void UNoiseListenerComponent::OnNoiseHeard(const int& _noiseVolume, const FVector& _pos)
{
	if (m_callBackFunc == nullptr) { return; }
	m_callBackFunc(_noiseVolume, _pos);
}



//----------------------------
//ƒR[ƒ‹ƒoƒbƒNŠÖ”‚Ì“o˜^
//-----------------------------
void UNoiseListenerComponent::SetCallBackFunction( const std::function<void(const int& _noiseVolume, const FVector& _pos)>& _callbackFunc)
{
	if (_callbackFunc == nullptr) { return; }
	//ƒR[ƒ‹ƒoƒbƒNŠÖ”‚É“o˜^
	m_callBackFunc = _callbackFunc;
	
}

//----------------------------
//EndPlay
//----------------------------
void UNoiseListenerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//ƒmƒCƒYƒ}ƒl[ƒWƒƒ[‚©‚çíœ
	UNoiseManager* manager = (GetWorld() ? GetWorld()->GetSubsystem<UNoiseManager>() : nullptr);
	if (manager)
	{
		manager->RemoveListener(this);
	}
}
