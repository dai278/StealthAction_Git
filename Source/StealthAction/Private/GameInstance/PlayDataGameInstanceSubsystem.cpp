//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/PlayDataGameInstanceSubsystem.h"
#include "Enemy/EnemyBase.h"

void UPlayDataGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// ‰Šú‰»ˆ—
}

void UPlayDataGameInstanceSubsystem::Deinitialize()
{
	// I—¹ˆ—
	Super::Deinitialize();
}

void UPlayDataGameInstanceSubsystem::BeginPlayHandler(UWorld* World)
{
	// ƒ[ƒ‹ƒhŠJn‚Ìˆ—
}

//------------------------------------------------
//ƒXƒe[ƒWƒNƒŠƒA‚µ‚½‚ÉŒÄ‚Î‚ê‚éŠÖ”
//------------------------------------------------
 void UPlayDataGameInstanceSubsystem::OnStageClear(const FName& _levelName)
{
	//ƒNƒŠƒA‚µ‚½ƒXƒe[ƒW–¼‚ğ•Û‘¶
	m_playDataInfo.ClearLevelNames.AddUnique(_levelName);

	ResetCheckpointInfo();
	ResetPlayerInfo();
	ResetEnemyInfo();
	m_playDataInfo.IsContinued = false;
}

 //------------------------------------------------
 //ƒXƒe[ƒWƒZƒŒƒNƒg‚É‚à‚Ç‚Á‚½‚ÉŒÄ‚Î‚ê‚éŠÖ”
 //------------------------------------------------
 void UPlayDataGameInstanceSubsystem::OnReturnToStageSelect()
 {
	 ResetCheckpointInfo();
	 ResetPlayerInfo();
	 ResetEnemyInfo();
	 m_playDataInfo.IsContinued = false;
 }

//------------------------------------------------
//ƒRƒ“ƒeƒjƒ…[‚ÉŒÄ‚Î‚ê‚éŠÖ”
//------------------------------------------------
 void UPlayDataGameInstanceSubsystem::OnContinue()
{
	 m_playDataInfo.IsContinued = true;
}


//------------------------------------------------
//ƒGƒlƒ~[î•ñæ“¾

FEnemyInfo UPlayDataGameInstanceSubsystem::GetEnemyInfo(const int32 _index)const
{
	
	int num = m_playDataInfo.EnemyInfo.Num();
	for (int i = 0; i < num; ++i)
	{
		if (m_playDataInfo.EnemyInfo[i].index == _index)
		{
			return m_playDataInfo.EnemyInfo[i];
		}
	}
	//ƒCƒ“ƒfƒbƒNƒX‚ª‚»‚ë‚¤‚Ì‚ª‚È‚¯‚ê‚Î‰Šúó‘Ô‚ğ•Ô‚·
	return FEnemyInfo{};
}
