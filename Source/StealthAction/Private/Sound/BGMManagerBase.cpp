// Fill out your copyright notice in the Description page of Project Settings.

#include "Sound/BGMManagerBase.h"
#include "TimerManager.h"

ABGMManagerBase::ABGMManagerBase()
{
	PrimaryActorTick.bCanEverTick = false;

	//==============================
	// AudioComponent作成
	//==============================

	NormalAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("NormalAudioComp"));
	CombatAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("CombatAudioComp"));
	CombatStartSEComp = CreateDefaultSubobject<UAudioComponent>(TEXT("CombatStartSEComp"));

	RootComponent = NormalAudioComp;

	CombatAudioComp->SetupAttachment(RootComponent);
	CombatStartSEComp->SetupAttachment(RootComponent);

	//自動再生OFF
	NormalAudioComp->bAutoActivate = false;
	CombatAudioComp->bAutoActivate = false;
	CombatStartSEComp->bAutoActivate = false;
}

void ABGMManagerBase::BeginPlay()
{
	Super::BeginPlay();

	CombatEnemyCount = 0;

	//==============================
	// 通常BGMセット＆再生
	//==============================

	if (NormalBGM)
	{
		NormalAudioComp->SetSound(NormalBGM);
		NormalAudioComp->Play();
	}

	//==============================
	// 戦闘BGMセット
	//==============================

	if (CombatBGM)
	{
		CombatAudioComp->SetSound(CombatBGM);
	}

	//==============================
	// 戦闘開始SEセット
	//==============================

	if (CombatStartSE)
	{
		CombatStartSEComp->SetSound(CombatStartSE);
	}
}


//==============================
// 戦闘開始
//==============================

void ABGMManagerBase::OnCombatStart()
{
	CombatEnemyCount++;

	//初回のみ
	if (CombatEnemyCount == 1)
	{
		//通常BGM FadeOut
		if (NormalAudioComp && NormalAudioComp->IsPlaying())
		{
			NormalAudioComp->FadeOut(1.0f, 0.0f);
		}

		//びっくり音再生（単発）
		if (CombatStartSEComp && CombatStartSE)
		{
			CombatStartSEComp->Play();
		}

		//戦闘BGM再生（同時）
		if (CombatAudioComp && CombatBGM)
		{
			if (!CombatAudioComp->IsPlaying())
			{
				CombatAudioComp->Play();
			}

			CombatAudioComp->FadeIn(1.0f, 1.0f);
		}
	}
}


//==============================
// CombatBGM開始処理
//==============================

void ABGMManagerBase::StartCombatBGM()
{
	if (CombatAudioComp && CombatBGM)
	{
		if (!CombatAudioComp->IsPlaying())
		{
			CombatAudioComp->Play();
		}

		CombatAudioComp->FadeIn(1.0f, 1.0f);
	}
}


//==============================
// 戦闘終了
//==============================

void ABGMManagerBase::OnCombatEnd()
{
	CombatEnemyCount--;

	if (CombatEnemyCount <= 0)
	{
		CombatEnemyCount = 0;

		//CombatBGM FadeOut
		if (CombatAudioComp && CombatAudioComp->IsPlaying())
		{
			CombatAudioComp->FadeOut(1.0f, 0.0f);
		}

		//通常BGM再開
		if (NormalAudioComp && NormalBGM)
		{
			if (!NormalAudioComp->IsPlaying())
			{
				NormalAudioComp->Play();
			}

			NormalAudioComp->FadeIn(1.0f, 1.0f);
		}
	}
}
