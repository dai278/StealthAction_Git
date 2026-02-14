//// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "BGMManagerBase.generated.h"

UCLASS()
class STEALTHACTION_API ABGMManagerBase : public AActor
{
	GENERATED_BODY()

public:
	ABGMManagerBase();

protected:
	virtual void BeginPlay() override;

	//=======================================
	//BGM
	//=======================================

	UPROPERTY(EditAnywhere, Category = "BGM")
	USoundCue* NormalBGM;

	UPROPERTY(EditAnywhere, Category = "BGM")
	USoundCue* CombatBGM;

	UPROPERTY(EditAnywhere, Category = "BGM")
	USoundCue* CombatStartSE;

	UPROPERTY()
	UAudioComponent* NormalAudioComp;

	UPROPERTY()
	UAudioComponent* CombatAudioComp;

	UPROPERTY()
	UAudioComponent* CombatStartSEComp;

	//=======================================
	//êÌì¨ä«óù
	//=======================================

	UPROPERTY()
	int32 CombatEnemyCount;

	UFUNCTION()
	void StartCombatBGM();

public:

	void OnCombatStart();

	void OnCombatEnd();
};

