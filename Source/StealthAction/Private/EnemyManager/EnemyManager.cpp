//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager/EnemyManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Enemy/EnemyBase.h"
#include "GameInstance/PlayDataGameInstanceSubsystem.h"
#include "Checkpoint/CheckpointManager.h"
#include "Kismet/GameplayStatics.h"

//------------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//------------------------------------------------
UEnemyManager::UEnemyManager()
{
	
}

//------------------------------------------------
// BeginPlayŠÖ”
//------------------------------------------------
void UEnemyManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (!GetWorld()) { return; }

	//ƒCƒ“ƒQ[ƒ€ƒŒƒxƒ‹‚Å‚È‚¯‚ê‚Îˆ—‚µ‚È‚¢
	FString name = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);
	if (name == "Title") { return; }
	if (name == "GameOver") { return; }
	if (name == "GameClear") { return; }


	//ƒGƒlƒ~[”z—ñ‚ğ‹ó‚É
	m_pAllEnemies.Empty();
	//ƒ`ƒFƒbƒNƒ|ƒCƒ“ƒgƒ}ƒl[ƒWƒƒ\‚ÉŠÖ”‚ğ“o˜^
	UCheckpointManager* pCheckMng = (GetWorld() ? GetWorld()->GetSubsystem<UCheckpointManager>() : nullptr);
	if (pCheckMng)
	{
		pCheckMng->AddOnOverlapCheckpointEvent(CreateOverlapCheckpointEvent(UEnemyManager::RegisterEnemyInfoAllToPlayData));
	}

	//Tick‚Ì1ƒtƒŒ[ƒ€–Ú‚ÉŒÄ‚Î‚ê‚éŠÖ”
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEnemyManager::SetEnemyInfoAll);

}

//------------------------------------------------
//ƒGƒlƒ~[“o˜^ŠÖ”
//------------------------------------------------
void UEnemyManager::RegisterEnemy(AEnemyBase* _pEnemy)
{
	if (!IsValid(_pEnemy)) { return; }
	m_pAllEnemies.AddUnique(_pEnemy);
	if (!_pEnemy)
	{
	}
	else
	{
	}
}

//------------------------------------------------
//ƒGƒlƒ~[íœŠÖ”
//------------------------------------------------
void UEnemyManager::UnregisterEnemy(AEnemyBase* _pEnemy)
{
	m_pAllEnemies.Remove(_pEnemy);
	if (!_pEnemy)
	{
	}
	else
	{
	}
}

//-----------------------------------------------
//ƒGƒlƒ~[‚ªŒÂl‚ª‚Á‚Ä‚¢‚éƒGƒlƒ~[‚ÌƒAƒhƒŒƒX“o˜^‰ğœ
//--------------------------------------------------
void UEnemyManager::UnregisteredOtherEnemyBaseAll(AEnemyBase* _pEnemy)
{
	//‘SƒGƒlƒ~[‚ğŠm”F
	for (AEnemyBase* pEnemy : m_pAllEnemies)
	{
		if (!pEnemy) { continue; }
		if (!IsValid(pEnemy)) { continue; }

		//ƒGƒlƒ~[‚ªŒÂl‚ª‚Á‚Ä‚¢‚éƒGƒlƒ~[‚ÌƒAƒhƒŒƒX“o˜^‰ğœ
		pEnemy->UnregisteredOtherEnemyBase(_pEnemy);	
	}
}


//------------------------------------------------
//‘SƒGƒlƒ~[î•ñ‚ğPlayData‚É‹L˜^‚·‚é
//------------------------------------------------
void UEnemyManager::RegisterEnemyInfoAllToPlayData()
{

	//ƒCƒ“ƒQ[ƒ€ƒŒƒxƒ‹‚Å‚È‚¯‚ê‚Îˆ—‚µ‚È‚¢
	FString name = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);
	if (name == "Title") { return; }
	if (name == "GameOver") { return; }
	if (name == "GameClear") { return; }

	//ƒvƒŒƒCƒf[ƒ^‚Ìæ“¾
	UPlayDataGameInstanceSubsystem* pPlayData = (GetWorld() && GetWorld()->GetGameInstance() ? GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>() : nullptr);
	if (!pPlayData)
	{
		return;
	}

	TArray<FEnemyInfo> enemyInfos;
	//‘SƒGƒlƒ~[‚©‚çenemyInfo‚ğæ“¾
	for (AEnemyBase* pEnemy : m_pAllEnemies)
	{
		if (IsValid(pEnemy)) { enemyInfos.Add(pEnemy->GetEnemyInfo()); }
	}
	//ƒvƒŒƒCƒf[ƒ^‚É‹L˜^
	pPlayData->SetEnemyInfo(enemyInfos);
}

//------------------------------------------------
//ƒGƒlƒ~[‚ªƒvƒŒƒCƒ„[”­Œ©‚µ‚Ä‚¢‚é‚©æ“¾ŠÖ”
//------------------------------------------------
bool UEnemyManager::IsAnyEnemyPlayerFound() const
{
	//‘SƒGƒlƒ~[‚ğŠm”F
	for (AEnemyBase* pEnemy : m_pAllEnemies)
	{
		if (!pEnemy) { continue; }
		if (!IsValid(pEnemy)) { continue; }

		//ƒvƒŒƒCƒ„[”­Œ©‚µ‚Ä‚¢‚éƒGƒlƒ~[‚ª‚¢‚ê‚Îtrue‚ğ•Ô‚·
		if (pEnemy->IsPlayerFound())
		{
			return true;
		}
	}
	return false;
}


//------------------------------------------------
//ˆê”Ô‹ß‚¢ƒGƒlƒ~[æ“¾ŠÖ
//------------------------------------------------
AEnemyBase* UEnemyManager::GetNearestEnemy(const FVector& _pos,  const float& _maxDistance) 
{

	//ˆê”Ô‹ß‚¢ƒGƒlƒ~[‚ğ’T‚·
	//return—p•Ï”
	AEnemyBase* pNearestEnemy = nullptr;
	//Å’Z‹——£•Û‘¶‚·‚é‚æ‚¤•Ï”,‰Šú’l‚ÍÅ‘å’l
	float nearestDistance = TNumericLimits<float>::Max();
	//ƒGƒŠƒA“à‚ÌƒGƒlƒ~[‘S‚Ä‚ğŠm”F
	for (AEnemyBase* pEnemy : m_pAllEnemies)
	{
		if (!pEnemy) { continue; }
		if (!IsValid(pEnemy)) { continue; }
		//ƒGƒlƒ~[‚Æw’èÀ•W‚Ì‹——£‚ğŒvZ
		const float distance = FVector::Dist(pEnemy->GetActorLocation(), _pos);
		//Å’Z‹——£‚æ‚è‹ß‚­ŒŸ’m‚ÌÅ‘å‹——£‚æ‚è‹ß‚¯‚ê‚ÎXV
		if (distance < nearestDistance)
		{
			nearestDistance = distance;
			if (distance < _maxDistance) {
				pNearestEnemy = pEnemy;
			}
		}
	}
	return pNearestEnemy;
}


//----------------------------------------------
//‘OƒGƒlƒ~[‚ÉEnemyInfo‚ğ“o˜^‚·‚éŠÖ”
//----------------------------------------------
void UEnemyManager::SetEnemyInfoAll()
{
	//ƒvƒŒƒCƒf[ƒ^‚Ìæ“¾
	UPlayDataGameInstanceSubsystem* pPlayData = (GetWorld() && GetWorld()->GetGameInstance() ? GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>() : nullptr);
	if (!pPlayData) { return; }
	if (!pPlayData->IsIsContinued()) { 
		return;
	}
	//ƒRƒ“ƒeƒjƒ…[‚µ‚Ä‚¢‚ê‚Î
	int32 num = m_pAllEnemies.Num();
	
	TArray<FEnemyInfo> info = pPlayData->GetAllEnemyInfo();
	int32 infoNum = info.Num();
	if (num != infoNum) { return; }

	//ƒCƒ“ƒfƒbƒNƒX‚ª“¯‚¶ƒGƒlƒ~[‚Éî•ñİ’è
	for (int i = 0;i < m_pAllEnemies.Num();++i)
	{
		if (!IsValid(m_pAllEnemies[i])) { continue; }
		for (int n = 0;n < info.Num();++n)
		{
			if (m_pAllEnemies[i]->GetEnemyInfo().index == info[n].index)
			{
				//î•ñİ’è
				m_pAllEnemies[i]->SetEnemyInfo(info[n]);
				//€‚ñ‚Å‚é“G‚Í€‚ñ‚¾ˆÊ’u‚ÉƒŠƒXƒ|[ƒ“
				//¶‚«‚Ä‚¢‚é‚Ì‚Í‰ŠúˆÊ’u
				if (m_pAllEnemies[i]->GetEnemyInfo().bIsDead) {
					m_pAllEnemies[i]->SetActorLocation(info[n].location);
				}
				break;
			}
		}
	}
}

