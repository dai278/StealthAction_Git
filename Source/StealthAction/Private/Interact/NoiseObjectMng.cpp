//íSìñì˙Ó‡
// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/NoiseObjectMng.h"
#include "Interact/NoiseObject.h"

//----------------------------------------------------------
//ÉRÉìÉXÉgÉâÉNÉ^
//----------------------------------------------------------
UNoiseObjectMng::UNoiseObjectMng()
	:UWorldSubsystem()
{
}
//----------------------------------------------------------
//ÉxÉMÉìÉvÉåÉC
//----------------------------------------------------------
void UNoiseObjectMng::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}
//----------------------------------------------------------
//âπÉIÉuÉWÉFÉNÉgìoò^
//----------------------------------------------------------
void UNoiseObjectMng::RegisterNoiseObject(ANoiseObject* _pNoiseObject)
{
	if (!IsValid(_pNoiseObject)) { return; }
	m_noiseObjects.AddUnique(_pNoiseObject);
}
//----------------------------------------------------------
//âπÉIÉuÉWÉFÉNÉgìoò^âèú
//----------------------------------------------------------
void UNoiseObjectMng::UnregisterNoiseObject(ANoiseObject* _pNoiseObject)
{
	if (!_pNoiseObject) { return; }
	m_noiseObjects.RemoveSingle(_pNoiseObject);
}

//----------------------------------------------------------
//âπî≠ê∂
//----------------------------------------------------------	
void UNoiseObjectMng::MakeNoise(const int32& _noiseLevel, const FVector& _noiseLocation, const TArray<int32>& _index)
{
	for (int32 index : _index)
	{
		for (ANoiseObject* noiseObj : m_noiseObjects)
		{
			if (!IsValid(noiseObj)) { continue; }
			//âπÉIÉuÉWÉFÉNÉgÇ…âπî≠ê∂éwé¶
			if (noiseObj->GetIndex() == index) {
				noiseObj->MakeNoise();
			}
		}
	}
}