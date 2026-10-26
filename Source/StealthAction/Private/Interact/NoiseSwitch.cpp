//íSìñì˙Ó‡
// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/NoiseSwitch.h"
#include "Engine/World.h"
#include "Interact/NoiseObjectMng.h"

//----------------------------------------------------------
//ÉRÉìÉXÉgÉâÉNÉ^
//----------------------------------------------------------
ANoiseSwitch::ANoiseSwitch()
	:AInteract()
	, m_bIsOn(false)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}


//----------------------------------------------------------
//ÉxÉMÉìÉvÉåÉC
//----------------------------------------------------------
void ANoiseSwitch::BeginPlay()
{
	Super::BeginPlay();
	
}

//----------------------------------------------------------
//çXêV
//----------------------------------------------------------
void ANoiseSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//----------------------------------------------------------
//ÉCÉìÉ^ÉâÉNÉg
//----------------------------------------------------------
void ANoiseSwitch::Interact(AActor* _interactOwner)
{
	if (m_bIsOn) { return; }

	m_bIsOn = true;
	//ëõâπÇèoÇ∑ÉIÉuÉWÉFÉNÉgÇ…í ím
	UNoiseObjectMng* noiseMng = (GetWorld() ? GetWorld()->GetSubsystem<UNoiseObjectMng>() : nullptr);

	if (noiseMng)
	{
		noiseMng->MakeNoise(1, GetActorLocation(), m_makeNoiseObjectIndex);
	}
}

//----------------------------------------------------------
//ÉCÉìÉ^ÉâÉNÉgÅiÉ|Å[ÉYÇ∑ÇÈÇ©Ç«Ç§Ç©Åj
//----------------------------------------------------------
void ANoiseSwitch::Interact(AActor* _interactOwner, bool& _isPose)
{
	Interact(_interactOwner);
	_isPose = false;
}