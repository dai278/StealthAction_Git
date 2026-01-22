// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/NoiseSwitch.h"
#include "Interact/NoiseObjectMng.h"

//----------------------------------------------------------
//コンストラクタ
//----------------------------------------------------------
ANoiseSwitch::ANoiseSwitch()
	:AInteract()
	, m_bIsOn(false)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}


//----------------------------------------------------------
//ベギンプレイ
//----------------------------------------------------------
void ANoiseSwitch::BeginPlay()
{
	Super::BeginPlay();
	
}

//----------------------------------------------------------
//更新
//----------------------------------------------------------
void ANoiseSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//----------------------------------------------------------
//インタラクト
//----------------------------------------------------------
void ANoiseSwitch::Interact(AActor* _interactOwner)
{
	if (m_bIsOn) { return; }

	m_bIsOn = true;
	//騒音を出すオブジェクトに通知
	UNoiseObjectMng* noiseMng = GetWorld()->GetSubsystem<UNoiseObjectMng>();

	if (noiseMng)
	{
		noiseMng->MakeNoise(1, GetActorLocation(), m_makeNoiseObjectIndex);
	}
}