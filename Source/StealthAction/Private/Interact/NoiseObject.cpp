//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/NoiseObject.h"
#include "Engine/World.h"
#include "Noise/NoiseManager.h"
#include "Interact/NoiseObjectMng.h"

//----------------------------------------------------------
//ƒRƒ“ƒXƒgƒ‰ƒNƒ^
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

	
	m_pNoiseManager = (GetWorld() ? GetWorld()->GetSubsystem<UNoiseManager>() : nullptr);
	

	UNoiseObjectMng* noiseObjMng = (GetWorld() ? GetWorld()->GetSubsystem<UNoiseObjectMng>() : nullptr);
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
	if (!IsValid(m_pNoiseManager)) { return; }

	
	//”­¶’x‰„ŽžŠÔ’†
	if(m_bIsInDelay)
	{
		m_noiseTimer += DeltaTime;
		//’x‰„ŽžŠÔI—¹”»’è
		if (m_noiseTimer >= m_noiseDelayTime)
		{
			m_bIsInDelay = false;
			m_bIsMakingNoise = true;
			m_noiseTimer = 0.f;
			m_pNoiseManager->MakeNoise(m_noiseLevel, GetActorLocation());
		}
		return;
	}
	
	//‰¹‚ðo‚µ‚Ä‚¢‚éŠÔ
	if (!m_bIsMakingNoise) { return; }
	//‰¹‚ðo‚µ‘±‚¯‚é
	m_pNoiseManager->MakeNoise(m_noiseLevel, GetActorLocation());
	//ŽžŠÔ”»’è
	if (m_noiseTimer >= m_noiseTime)
	{
		m_bIsMakingNoise = false;
		m_noiseTimer = 0.f;
		SetActorTickEnabled(false);
	}
}

//----------------------------------------------------------
//ƒCƒ“ƒ^ƒ‰ƒNƒg‚ÌŽÀ‘•
//----------------------------------------------------------
void ANoiseObject::Interact(AActor* _interactOwner)
{
	MakeNoise();
}

//ƒCƒ“ƒ^ƒ‰ƒNƒg‚ÌŽÀ‘•iƒ|[ƒY‚·‚é‚©‚Ç‚¤‚©j
void ANoiseObject::Interact(AActor* _interactOwner, bool& _isPose)
{
	Interact(_interactOwner);
	_isPose = false;
}


//----------------------------------------------------------
//‰¹‚ðo‚·ˆ—
//----------------------------------------------------------
void ANoiseObject::MakeNoise()
{
	if (!IsValid(m_pNoiseManager)) { return; }

	if (m_bIsMakingNoise || m_bIsInDelay) { return; }

	if (m_noiseDelayTime>0.f)
	{
		//’x‰„ŽžŠÔ‚ª‚ ‚éê‡A’x‰„ŽžŠÔ’†‚É‚·‚é
		m_bIsInDelay = true;
		m_noiseTimer = 0.f;
		SetActorTickEnabled(true);
	}
	else
	{
		//‘¦À‚É‰¹‚ðo‚·
		m_pNoiseManager->MakeNoise(m_noiseLevel, GetActorLocation());
		//Ž‘±“I‚É‰¹‚ðo‚·ê‡
		if (m_noiseTime > 0.f) {
			
			m_noiseTimer = 0.f;
			m_bIsMakingNoise = true;
			SetActorTickEnabled(true);
		}
	}

}