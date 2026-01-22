// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NoiseObjectMng.generated.h"

class ANoiseObject;
/**
 * 
 */
UCLASS()
class STEALTHACTION_API UNoiseObjectMng : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	UNoiseObjectMng();

protected:
	
	//ベギンプレイ
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:

	//音オブジェクト登録
	void RegisterNoiseObject(ANoiseObject* _pNoiseObject);
	//音オブジェクト登録解除
	void UnregisterNoiseObject(ANoiseObject* _pNoiseObject);
	//音発生
	void MakeNoise(const int32& _noiseLevel, const FVector& _noiseLocation,const TArray<int32>& _index);

private:
	TArray<ANoiseObject*> m_noiseObjects;
};
