// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/NoiseObjectMng.h"
#include "Interact/NoiseObject.h"

//----------------------------------------------------------
//コンストラクタ
//----------------------------------------------------------
UNoiseObjectMng::UNoiseObjectMng()
	:UWorldSubsystem()
{
}
//----------------------------------------------------------
//ベギンプレイ
//----------------------------------------------------------
void UNoiseObjectMng::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}
//----------------------------------------------------------
//音オブジェクト登録
//----------------------------------------------------------
void UNoiseObjectMng::RegisterNoiseObject(ANoiseObject* _pNoiseObject)
{
	if (!_pNoiseObject) { return; }
	m_noiseObjects.AddUnique(_pNoiseObject);
}
//----------------------------------------------------------
//音オブジェクト登録解除
//----------------------------------------------------------
void UNoiseObjectMng::UnregisterNoiseObject(ANoiseObject* _pNoiseObject)
{
	if (!_pNoiseObject) { return; }
	m_noiseObjects.RemoveSingle(_pNoiseObject);
}

//----------------------------------------------------------
//音発生
//----------------------------------------------------------	
void UNoiseObjectMng::MakeNoise(const int32& _noiseLevel, const FVector& _noiseLocation, const TArray<int32>& _index)
{
	for (int32 index : _index)
	{
		for (ANoiseObject* noiseObj : m_noiseObjects)
		{
			if (!noiseObj) { continue; }
			//音オブジェクトに音発生指示
			if (noiseObj->GetIndex() == index) {
				noiseObj->MakeNoise();
			}
		}
	}
}