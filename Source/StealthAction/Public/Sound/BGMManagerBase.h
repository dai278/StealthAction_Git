// Fill out your copyright notice in the Description page of Project Settings.

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

	//íBGM
	UPROPERTY(EditAnywhere, Category = "BGM")
	USoundCue* NormalBGM;

	//í“¬BGM
	UPROPERTY(EditAnywhere, Category = "BGM")
	USoundCue* CombatBGM;

	//AudioComponent(í)
	UPROPERTY()
	UAudioComponent* NormalAudioComp;

	//AudioComponent(í“¬)
	UPROPERTY()
	UAudioComponent* CombatAudioComp;

	//=======================================
	//í“¬ŠÇ—
	//=======================================

	//í“¬’†‚Ì“G‚Ì”
	UPROPERTY()
	int32 CombatEnemyCount;

public:
	//“G‚ªí“¬‚É“ü‚Á‚½‚Æ‚«ŒÄ‚Ô
	void OnCombatStart();

	//“G‚ªí“¬‚ğI—¹‚µ‚½‚Æ‚«ŒÄ‚Ô
	void OnCombatEnd();
};
