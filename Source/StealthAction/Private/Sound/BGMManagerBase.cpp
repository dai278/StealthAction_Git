// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/BGMManagerBase.h"

ABGMManagerBase::ABGMManagerBase()
{
	PrimaryActorTick.bCanEverTick = false;

	//AudioComponentçÏê¨
	NormalAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("NormalAudioComp"));
	CombatAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("CombatAudioComp"));

	RootComponent = NormalAudioComp;
	CombatAudioComp->SetupAttachment(RootComponent);

	//é©ìÆçƒê∂OFF
	NormalAudioComp->bAutoActivate = false;
	CombatAudioComp->bAutoActivate = false;
}

void ABGMManagerBase::BeginPlay()
{
	Super::BeginPlay();

	CombatEnemyCount = 0;

	//BGMÉZÉbÉg
	if (NormalBGM)
	{
		NormalAudioComp->SetSound(NormalBGM);
		NormalAudioComp->Play();
	}

	if (CombatBGM)
	{
		CombatAudioComp->SetSound(CombatBGM);
	}
}

void ABGMManagerBase::OnCombatStart()
{
	CombatEnemyCount++;

	//èââÒÇÃÇ›êÿÇËë÷Ç¶
	if (CombatEnemyCount == 1)
	{
		if (NormalAudioComp->IsPlaying())
		{
			NormalAudioComp->FadeOut(1.0f, 0.0f);
		}

		if (!CombatAudioComp->IsPlaying())
		{
			CombatAudioComp->Play();
		}

		CombatAudioComp->FadeIn(1.0f, 1.0f);
	}
}

void ABGMManagerBase::OnCombatEnd()
{
	CombatEnemyCount--;

	if (CombatEnemyCount <= 0)
	{
		CombatEnemyCount = 0;

		if (CombatAudioComp->IsPlaying())
		{
			CombatAudioComp->FadeOut(1.0f, 0.0f);
		}

		if (!NormalAudioComp->IsPlaying())
		{
			NormalAudioComp->Play();
		}

		NormalAudioComp->FadeIn(1.0f, 1.0f);
	}
}