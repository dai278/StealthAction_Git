// Fill out your copyright notice in the Description page of Project Settings.


#include "Noise/NoiseListenerComponent.h"
#include "Noise//NoiseManager.h"

//-----------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------
UNoiseListenerComponent::UNoiseListenerComponent()
	:m_callBackFunc(nullptr)
{
	//更新しない
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


//---------------------------------------------------------
// BeginPlay
//----------------------------------------------------------
void UNoiseListenerComponent::BeginPlay()
{
	Super::BeginPlay();

	//ノイズマネージャーに登録
	UNoiseManager* manager = GetWorld()->GetSubsystem<UNoiseManager>();
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
//コールバック関数の呼び出し
//--------------------------
void UNoiseListenerComponent::OnNoiseHeard(const int& _noiseVolume, const FVector& _pos)
{
	if (m_callBackFunc == nullptr) { return; }
	m_callBackFunc(_noiseVolume, _pos);
}



//----------------------------
//コールバック関数の登録
//-----------------------------
void UNoiseListenerComponent::SetCallBackFunction( const std::function<void(const int& _noiseVolume, const FVector& _pos)>& _callbackFunc)
{
	if (_callbackFunc == nullptr) { return; }
	//コールバック関数に登録
	m_callBackFunc = _callbackFunc;
	
}
