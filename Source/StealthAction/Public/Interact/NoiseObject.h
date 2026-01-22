// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/Interact.h"
#include "NoiseObject.generated.h"

class UNoiseManager;
/**
 * 
 */
UCLASS()
class STEALTHACTION_API ANoiseObject : public AInteract
{
	GENERATED_BODY()

public:
	ANoiseObject();

protected:
	void BeginPlay() override;

public:
	void Tick(float DeltaTime) override;

public:
	virtual void Interact(AActor* _interactOwner) override;

public:
	void MakeNoise();

private:
	UPROPERTY(EditAnywhere, Category = "Noise")
	int32 m_noiseLevel;//音の大きさ

	UPROPERTY(EditAnywhere, Category = "Noise")
	float m_noiseTime;//音を出す時間

	float m_noiseTimer;//音を出している時間カウント

	//音が出ているかどうか
	bool m_bIsMakingNoise;

	//音が出るまでの待機時間
	float m_noiseDelayTime;
	
	//待機時間中かどうか
	bool m_bIsInDelay;

	//音マネージャ―ポインタ
	UNoiseManager* m_pNoiseManager;
};