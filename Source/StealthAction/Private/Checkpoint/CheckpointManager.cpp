//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint/CheckpointManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Checkpoint/Checkpoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/PlayDataGameInstanceSubsystem.h"


//----------------------------------------------------------
//ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//----------------------------------------------------------
UCheckpointManager::UCheckpointManager()
{
	//‹ó‚É‚·‚é
	m_checkpoints.Empty();
}


//------------------------------------------------
// BeginPlayŠÖ”
//------------------------------------------------
void UCheckpointManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (!GetWorld()) { return; }

	//ƒCƒ“ƒQ[ƒ€ƒŒƒxƒ‹‚Å‚È‚¯‚ê‚Îˆ—‚µ‚È‚¢
	FString name = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);
	if (name == "Title") { return; }
	if (name == "GameOver") { return; }
	if (name == "GameClear") { return; }

	//‹ó‚É‚·‚é
	m_checkpoints.Empty();

	//ƒvƒŒƒCƒf[ƒ^‚Ìæ“¾
	UPlayDataGameInstanceSubsystem* pPlayData = (GetWorld() && GetWorld()->GetGameInstance() ? GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>() : nullptr);
	if (!pPlayData)
	{
		return;
	}
	FCheckpointInfo info= pPlayData->GetCheckpointInfo();
	m_currentCheckpointIndex = info.Index;

	//Tick‚Ì1ƒtƒŒ[ƒ€–Ú‚ÉŒÄ‚Î‚ê‚éŠÖ”
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UCheckpointManager::SetCheckPointAll);


}

//----------------------------------------------------------
//ƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚ğ“o˜^‚·‚éŠÖ”
//----------------------------------------------------------
void UCheckpointManager::AddCheckpoint(ACheckpoint* _checkpoint)
{
	if (!IsValid(_checkpoint))
	{
		return;
	}

	//ƒCƒ“ƒfƒbƒNƒX‚ª•s³‚È‚ç“o˜^‚µ‚È‚¢
	if(_checkpoint->GetIndex() <0)
	{
		return;
	}

	//ƒ‰ƒCƒg‚ğ’Ç‰Á‚·‚é—v‘f”‚ğ“Á’è
	for (int i = 0;i < m_checkpoints.Num(); ++i)
	{
		//‚±‚Ì—v‘f”‚Ìƒ‰ƒCƒg‚æ‚èƒCƒ“ƒfƒbƒNƒX‚ª’á‚¢‚È‚ç
		//‚±‚Ì—v‘f”‚É’Ç‰Á‚·‚é
		if (_checkpoint->GetIndex() == i)
		{
			//null‚È‚ç’Ç‰Á
			if (!IsValid(m_checkpoints[i])) {
				//TArray‚Éƒ‰ƒCƒg‚ğ’Ç‰Á
				m_checkpoints[i] = _checkpoint;
				return;
			}

			return;
		}

	}

	//‚±‚±‚Ü‚Å—ˆ‚½‚çŒ»İ‚Ì—v‘f”‚æ‚è‘å‚«‚¢ˆ×—v‘f”‚ğƒ‰ƒCƒgƒCƒ“ƒfƒbƒNƒX•ª‚Ü‚Å’Ç‰Á
	m_checkpoints.SetNum(_checkpoint->GetIndex() + 1);
	m_checkpoints[_checkpoint->GetIndex()] = _checkpoint;
}

//----------------------------------------------------------
//ƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚Ìæ“¾ŠÖ”
//----------------------------------------------------------
ACheckpoint* UCheckpointManager::GetCheckpoint(const int32 _index) const
{
	//ƒCƒ“ƒfƒbƒNƒX‚ª•s³‚È‚çnullptr‚ğ•Ô‚·
	if (_index < 0 || m_checkpoints.Num() <= _index)
	{
		return nullptr;
	}
	return IsValid(m_checkpoints[_index]) ? m_checkpoints[_index] : nullptr;
}

//----------------------------------------------------------
//Œ»İ‚Ìƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚ÌˆÊ’uæ“¾
//----------------------------------------------------------
FVector UCheckpointManager::GetCurrentCheckpointLocation() const
{
	
	//ƒCƒ“ƒQ[ƒ€ƒŒƒxƒ‹‚Å‚È‚¯‚ê‚Îˆ—‚µ‚È‚¢
	FString name = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefix=*/true);
	if (name == "Title"||name == "GameOver"||name == "GameClear") 
	{ 
		return FVector::ZeroVector;
	}

	for (ACheckpoint* check : m_checkpoints)
	{
		if (IsValid(check) && check->GetIndex() == m_currentCheckpointIndex)
		{
			return check->GetActorLocation();
		}
	}
	return FVector::ZeroVector;
}

//----------------------------------------------------------
//Œ»İ‚Ìƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚ğİ’è‚·‚éŠÖ”
//----------------------------------------------------------
void UCheckpointManager::SetCurrentCheckpointIndex(const int32& _index)
{
	//ƒCƒ“ƒfƒbƒNƒX‚ª0–¢–‚©”z—ñŠOAŒ»İ‚Ìƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚æ‚è¬‚³‚¢‚È‚çİ’è‚µ‚È‚¢
	if (!m_checkpoints.IsValidIndex(_index) || !IsValid(m_checkpoints[_index])
		|| _index < m_currentCheckpointIndex)
	{
		return;
	}
	if(_index < m_currentCheckpointIndex)
	{
		return;
	}
	//@•s³‚È’l‚Å‚È‚¯‚ê‚ÎƒCƒ“ƒfƒbƒNƒX‚ğİ’è
	m_currentCheckpointIndex = _index;

	//	//ƒvƒŒƒCƒf[ƒ^‚Ìæ“¾
	UPlayDataGameInstanceSubsystem* pPlayData = (GetWorld() && GetWorld()->GetGameInstance() ? GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>() : nullptr);
	if (!pPlayData)
	{
		return;
	}

	pPlayData->SetCheckpointInfo(m_checkpoints[_index]->GetCheckpointInfo());

}


//----------------------------------------------------------
//Œ»İ‚Ìƒ`ƒFƒbƒNƒ|ƒCƒ“ƒgƒCƒ“ƒfƒbƒNƒX‚ğæ“¾‚·‚éŠÖ”
//----------------------------------------------------------
int32 UCheckpointManager::GetCurrentCheckpointIndex() const
{
	return m_currentCheckpointIndex;
}

//-----------------------------------------------------------
//ƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚ÉG‚ê‚½Û‚ÉŒÄ‚Ño‚·ŠÖ”
//-----------------------------------------------------------
void UCheckpointManager::OnOverlapCheckpoint()
{
	int32 num = m_OnOverlapCheckpointFunctions.Num();
	for (std::function<void()>& func : m_OnOverlapCheckpointFunctions)
	{
		if (func) { func(); }
	}
}

//---------------------------------------------------------
//‘Sƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚Ìİ’è
//---------------------------------------------------------
void UCheckpointManager::SetCheckPointAll()
{
	//ƒvƒŒƒCƒf[ƒ^‚Ìæ“¾
	UPlayDataGameInstanceSubsystem* pPlayData = (GetWorld() && GetWorld()->GetGameInstance() ? GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>() : nullptr);
	if (!pPlayData) { return; }
	if (!pPlayData->IsIsContinued()) {
		return;
	}

	for (ACheckpoint* check : m_checkpoints)
	{
		//Œ»İ‚ÌƒCƒ“ƒfƒbƒNƒXˆÈ‰º‚Í”ñƒAƒNƒeƒBƒu‚É
		if (IsValid(check) && check->GetIndex() <= m_currentCheckpointIndex)
		{
			check->SetHasCheckpoint(false);
		}
	}

}