//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Light/ExtendedSpotLightManager.h"
#include "Light/ExtendedSpotLight.h"

//----------------------------------------
//BeginPlay
//----------------------------------------
void UExtendedSpotLightManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	//ƒ‰ƒCƒg”z—ñ‚ğ‹ó‚É
	m_pLights.Empty();
	//“Gƒ‰ƒCƒg”z—ñ‚ğ‹ó‚É
	m_pEnemyLights.Empty();
}


//-------------------------------------------
//ƒ‰ƒCƒg‚Ì’Ç‰Á
//-------------------------------------------
void UExtendedSpotLightManager::AddLight(AExtendedSpotLight* _pLight)
{
	if (!IsValid(_pLight)) { return; }


	if (!_pLight->IsShadowDrawLight()) { 
		//-1‚È‚ç“Gƒ‰ƒCƒg”z—ñ‚É’Ç‰Á
			m_pEnemyLights.AddUnique(_pLight);
		return; 
	}


	if (_pLight->GetIndex() < 0) { return; }

	//ƒ‰ƒCƒg‚ğ’Ç‰Á‚·‚é—v‘f”‚ğ“Á’è
	for (int i = 0;i < m_pLights.Num(); ++i)
	{
		//‚±‚Ì—v‘f”‚Ìƒ‰ƒCƒg‚æ‚èƒCƒ“ƒfƒbƒNƒX‚ª’á‚¢‚È‚ç
		//‚±‚Ì—v‘f”‚É’Ç‰Á‚·‚é
		if (_pLight->GetIndex() ==i)
		{
			//null‚È‚ç’Ç‰Á
			if (!IsValid(m_pLights[i])) {
				//TArray‚Éƒ‰ƒCƒg‚ğ’Ç‰Á
				m_pLights[i]=_pLight;
				return;
			}
			
			return;
		}

	}

	//‚±‚±‚Ü‚Å—ˆ‚½‚çŒ»İ‚Ì—v‘f”‚æ‚è‘å‚«‚¢ˆ×—v‘f”‚ğƒ‰ƒCƒgƒCƒ“ƒfƒbƒNƒX‚Ü‚Å’Ç‰Á
	m_pLights.SetNum(_pLight->GetIndex()+1);
	m_pLights[_pLight->GetIndex()] = _pLight;
}


//ƒ‰ƒCƒgíœ
void UExtendedSpotLightManager::UnRegisterLight(AExtendedSpotLight* _pLight)
{
	if (!_pLight) { return; }
	m_pEnemyLights.Remove(_pLight);
	for (AExtendedSpotLight*& Light : m_pLights)
	{
		if (Light == _pLight) { Light = nullptr; }
	}
}



//ƒ‰ƒCƒg‚Ìæ“¾
AExtendedSpotLight* UExtendedSpotLightManager::GetLight(const int32 _lightIndex)
{
	//ƒCƒ“ƒfƒbƒNƒX‚ª—LŒø”ÍˆÍ“à‚È‚ç‚»‚ÌƒCƒ“ƒfƒbƒNƒX‚Ìƒ‰ƒCƒg‚ÌƒAƒhƒŒƒX‚ğ•Ô‚·
	if (m_pLights.Num() > _lightIndex&&_lightIndex>=0) {
		
		return IsValid(m_pLights[_lightIndex]) ? m_pLights[_lightIndex] : nullptr;
	}
	else
	{

		return nullptr;
	}
}


//---------------------
//ƒ‰ƒCƒg‚Æ‚ÌÕ“Ëƒ`ƒFƒbƒN
//-------------------
bool UExtendedSpotLightManager::IsHitAllLight(const FVector& _pos)const
{
	for (int i = 0; i < m_pLights.Num(); ++i)
	{
		if (IsValid(m_pLights[i]) && m_pLights[i]->IsHit(_pos))
		{
			return true;
		}
	}
	for(int i = 0; i < m_pEnemyLights.Num(); ++i)
	{
		if (IsValid(m_pEnemyLights[i]) && m_pEnemyLights[i]->IsHit(_pos))
		{
			return true;
		}
	}

	return false;
}

//---------------------
//“Gƒ‰ƒCƒg‚Æ‚ÌÕ“Ëƒ`ƒFƒbƒN
//-------------------
bool UExtendedSpotLightManager::IsHitEnemyLight(const FVector& _pos)const
{
	for (int i = 0; i < m_pEnemyLights.Num(); ++i)
	{
		if (!IsValid(m_pEnemyLights[i])) { continue; }
		
		if (m_pEnemyLights[i]->IsHit(_pos))
		{
			return true;
		}
	}
	return false;
}

//-----------------------
//‰e‚ğ—‚Æ‚·ƒ‰ƒCƒg‚Æ‚ÉÆ‚ç‚³‚ê‚Ä‚¢‚éˆÈ‰º
//------------------------
bool UExtendedSpotLightManager::IsHitShadowDrawLight(const FVector& _pos)const
{
	for (int i = 0; i<m_pLights.Num(); ++i)
	{
		if (!IsValid(m_pLights[i])) { continue; }
		if (m_pLights[i]->IsHit(_pos))
		{
			return true;
		}
	}
	return false;
}


void UExtendedSpotLightManager::SwitchTurnLight(const int32& _index)
{
	for (AExtendedSpotLight* p : m_pEnemyLights)
	{
		if (IsValid(p))
		{
			if (p->GetIndex() == _index)
			{
				p->SwitchTurn();
			}
		}
	}

	for (AExtendedSpotLight* p : m_pLights)
	{
		if (IsValid(p))
		{
			if (p->GetIndex() == _index)
			{
				p->SwitchTurn();
			}
		}
	}

}


void UExtendedSpotLightManager::OnRotateTargetAngleStop(const int32& _index)
{
	for (AExtendedSpotLight* p : m_pEnemyLights)
	{
		if (IsValid(p))
		{
			if (p->GetIndex() == _index)
			{
				p->OnRotateTargetAngleStop();
			}
		}
	}

	for (AExtendedSpotLight* p : m_pLights)
	{
		if (IsValid(p))
		{
			if (p->GetIndex() == _index)
			{
				p->OnRotateTargetAngleStop();
			}
		}
	}
}
