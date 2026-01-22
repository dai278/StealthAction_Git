// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/NoiseObject.h"
#include "Noise/NoiseManager.h"
#include "Interact/NoiseObjectMng.h"

//----------------------------------------------------------
//コンストラクタ
//----------------------------------------------------------
ANoiseObject::ANoiseObject()
	:AInteract()
	, m_noiseLevel(1)
	, m_noiseTime(2.f)
	, m_noiseTimer(0.f)
	, m_bIsMakingNoise(false)
	, m_noiseDelayTime(0.f)
	, m_bIsInDelay(false)
	, m_pNoiseManager(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

}

//----------------------------------------------------------
//BeginPlay
//----------------------------------------------------------
void ANoiseObject::BeginPlay()
{
	Super::BeginPlay();

	
	m_pNoiseManager = GetWorld()->GetSubsystem<UNoiseManager>();
	

	UNoiseObjectMng* noiseObjMng = GetWorld()->GetSubsystem<UNoiseObjectMng>();
	if (noiseObjMng)
	{
		noiseObjMng->RegisterNoiseObject(this);
	}
}

//----------------------------------------------------------
//Tick
//----------------------------------------------------------
void ANoiseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//発生遅延時間中
	if(m_bIsInDelay)
	{
		m_noiseTimer += DeltaTime;
		//遅延時間終了判定
		if (m_noiseTimer >= m_noiseDelayTime)
		{
			m_bIsInDelay = false;
			m_bIsMakingNoise = true;
			m_noiseTimer = 0.f;
			m_pNoiseManager->MakeNoise(m_noiseLevel, GetActorLocation());
		}
		return;
	}
	
	//音を出している間
	if (!m_bIsMakingNoise) { return; }
	//音を出し続ける
	m_pNoiseManager->MakeNoise(m_noiseLevel, GetActorLocation());
	//時間判定
	if (m_noiseTimer >= m_noiseTime)
	{
		m_bIsMakingNoise = false;
		m_noiseTimer = 0.f;
		SetActorTickEnabled(false);
	}
}

//----------------------------------------------------------
//インタラクトの実装
//----------------------------------------------------------
void ANoiseObject::Interact(AActor* _interactOwner)
{
	MakeNoise();
}

//----------------------------------------------------------
//音を出す処理
//----------------------------------------------------------
void ANoiseObject::MakeNoise()
{
	if (!m_pNoiseManager) { return; }

	if (m_bIsMakingNoise || m_bIsInDelay) { return; }

	if (m_noiseDelayTime>0.f)
	{
		//遅延時間がある場合、遅延時間中にする
		m_bIsInDelay = true;
		m_noiseTimer = 0.f;
		SetActorTickEnabled(true);
	}
	else
	{
		//即座に音を出す
		m_pNoiseManager->MakeNoise(m_noiseLevel, GetActorLocation());
		//持続的に音を出す場合
		if (m_noiseTime > 0.f) {
			
			m_noiseTimer = 0.f;
			m_bIsMakingNoise = true;
			SetActorTickEnabled(true);
		}
	}

}