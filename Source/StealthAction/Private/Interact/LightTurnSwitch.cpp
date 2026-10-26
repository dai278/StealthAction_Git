//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/LightTurnSwitch.h"
#include "Engine/World.h"
#include "Light/ExtendedSpotLightManager.h"

//--------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//--------------------------
ALightTurnSwitch::ALightTurnSwitch()
{
}

//--------------------------
// ƒQ[ƒ€ŠJn‚ÉŒÄ‚Î‚ê‚é
//--------------------------
void ALightTurnSwitch::BeginPlay()
{
	Super::BeginPlay();

}

//--------------------------
// –ˆƒtƒŒ[ƒ€ŒÄ‚Î‚ê‚é
//--------------------------
void ALightTurnSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//--------------------------
// ƒCƒ“ƒ^ƒ‰ƒNƒgˆ—
//--------------------------
void ALightTurnSwitch::Interact(AActor* _interactOwner)
{
	
	if (m_indexs.Num() <= 0) { return; }
	UExtendedSpotLightManager* lightMng = (GetWorld() ? GetWorld()->GetSubsystem<UExtendedSpotLightManager>() : nullptr);
	if (lightMng)
	{
		for (int32 index : m_indexs)
		{
			lightMng->OnRotateTargetAngleStop(index);

		}
	}
}

//--------------------------
// ƒCƒ“ƒ^ƒ‰ƒNƒgˆ—iƒ|[ƒY‚·‚é‚©‚Ç‚¤‚©j
//--------------------------
void ALightTurnSwitch::Interact(AActor* _interactOwner, bool& _isPose)
{
	Interact(_interactOwner);
	_isPose = false;
}