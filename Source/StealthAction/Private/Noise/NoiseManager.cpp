//’S“–“úîà
//----------------------------------------------
//ƒvƒŒƒCƒ„[‚ª–Â‚ç‚µ‚½‰¹‚ğŠÇ—‚·‚éƒNƒ‰ƒX
//----------------------------------------------

#include "Noise/NoiseManager.h"
#include "Noise/NoiseListenerComponent.h"


//----------------------------
//BeginPlay
//----------------------------
void UNoiseManager:: OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	//”z—ñ‚ğ‹ó‚É
	m_noiseListeners.Empty();
}

//-------------------------------
//‰¹‚ğ–Â‚ç‚·
//-------------------------------
void UNoiseManager::MakeNoise(const int& _noiseVolume, const FVector& _pos)
{
	for (int i = 0;i < m_noiseListeners.Num();++i)
	{
		//null‚È‚çˆ—‚µ‚È‚¢
		if (!IsValid(m_noiseListeners[i])) { continue; }
		//ƒAƒNƒeƒBƒu‚Å‚È‚¯‚ê‚Îˆ—‚µ‚È‚¢
		if (!m_noiseListeners[i]->IsActive()) { continue; }
		m_noiseListeners[i]->OnNoiseHeard(_noiseVolume, _pos);
	}
}

//------------------------------
//ƒŠƒXƒi[‚Ì’Ç‰Á
//------------------------------
void UNoiseManager::AddListener(UNoiseListenerComponent* _listener)
{
	if (!IsValid(_listener)) { return; }
		m_noiseListeners.AddUnique(_listener);
}

//------------------------------
//ƒŠƒXƒi[‚Ìíœ
//------------------------------
void UNoiseManager::RemoveListener(UNoiseListenerComponent* _listener)
{
	if (_listener == nullptr) { return; }
	m_noiseListeners.Remove(_listener);
}