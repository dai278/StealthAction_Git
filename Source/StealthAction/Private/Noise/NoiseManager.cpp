//----------------------------------------------
//プレイヤーが鳴らした音を管理するクラス
//----------------------------------------------

#include "Noise/NoiseManager.h"
#include "Noise/NoiseListenerComponent.h"


//----------------------------
//BeginPlay
//----------------------------
void UNoiseManager:: OnWorldBeginPlay(UWorld&)
{
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
		if (m_noiseListeners[i] == nullptr) { return; }
		//アクティブでなければ処理しない
		if (!m_noiseListeners[i]->IsActive()) { return; }
		m_noiseListeners[i]->OnNoiseHeard(_noiseVolume, _pos);
	}
}

//------------------------------
//リスナーの追加
//------------------------------
void UNoiseManager::AddListener(UNoiseListenerComponent* _listener)
{
	if (_listener == nullptr) { return; }
		m_noiseListeners.Add(_listener);
}