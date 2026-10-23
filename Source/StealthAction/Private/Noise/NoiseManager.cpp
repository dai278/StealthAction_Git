//----------------------------------------------
//プレイヤーが鳴らした音を管理するクラス
//----------------------------------------------

#include "Noise/NoiseManager.h"
#include "Noise/NoiseListenerComponent.h"


//----------------------------
//BeginPlay
//----------------------------
void UNoiseManager:: OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	//配列を空に
	m_noiseListeners.Empty();
}

//-------------------------------
//音を鳴らす
//-------------------------------
void UNoiseManager::MakeNoise(const int& _noiseVolume, const FVector& _pos)
{
	for (int i = 0;i < m_noiseListeners.Num();++i)
	{
		//nullなら処理しない
		if (!IsValid(m_noiseListeners[i])) { continue; }
		//アクティブでなければ処理しない
		if (!m_noiseListeners[i]->IsActive()) { continue; }
		m_noiseListeners[i]->OnNoiseHeard(_noiseVolume, _pos);
	}
}

//------------------------------
//リスナーの追加
//------------------------------
void UNoiseManager::AddListener(UNoiseListenerComponent* _listener)
{
	if (!IsValid(_listener)) { return; }
		m_noiseListeners.AddUnique(_listener);
}

//------------------------------
//リスナーの削除
//------------------------------
void UNoiseManager::RemoveListener(UNoiseListenerComponent* _listener)
{
	if (_listener == nullptr) { return; }
	m_noiseListeners.Remove(_listener);
}