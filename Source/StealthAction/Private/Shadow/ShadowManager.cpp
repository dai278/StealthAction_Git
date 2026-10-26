//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "Shadow\ShadowManager.h"
#include "Engine/World.h"
#include "Shadow/ShadowComponent.h"
#include "Light/ExtendedSpotLightManager.h"
#include "Light//ExtendedSpotLight.h"

//-------------------------------------
//ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//-------------------------------------
UShadowManager::UShadowManager()
{

}


//----------------------------------------
//BeginPlay
//----------------------------------------
void UShadowManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (!GetWorld()) { return; }

	//”z—ñ‚ğ‹ó‚É‚µ‚Ä’u‚­
	for (int i = 0;i < m_shadows.Num();++i)
	{
		m_shadows[i].Empty();
	}
	m_shadows.Empty();

	//ˆÊ’uƒtƒŒ[ƒ€Œã‚Éw’è‚ÌŠÖ”‚ğŒÄ‚Ño‚·ˆ—
	//ƒ‰ƒCƒg‚ªƒ‰ƒCƒgƒ}ƒl[ƒWƒƒ[‚É“o˜^‚ªI‚í‚Á‚½Œã‚É‰e‚Éƒ‰ƒCƒg‚ğ“o˜^‚µ‚½‚¢‚ªA
	//BeginPlay‚ÌŒÄ‚Ño‚µ‡‚ªSubSystem¨Actor‚Èˆ×
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UShadowManager::AllLinkLights);

}


//-------------------------------------------
//ƒ‰ƒCƒg‚ğ‰e‚Éİ’è‚·‚é
// Tick‚Ì‚PƒtƒŒ[ƒ€–Ú‚É‚Ì‚İŒÄ‚Î‚ê‚é
//------------------------------------------
void UShadowManager::AllLinkLights()
{
	//ƒ‰ƒCƒgƒ}ƒl[ƒWƒƒ[‚ÌƒAƒhƒŒƒX
	UExtendedSpotLightManager* pLightMng = (GetWorld() ? GetWorld()->GetSubsystem<UExtendedSpotLightManager>() : nullptr);

	//nullƒ`ƒFƒbƒN
	if (!pLightMng) { return; }

	for (int i = 0;i < m_shadows.Num();++i)
	{
		for (int n = 0;n < m_shadows[i].Num();++n)
		{
			//ƒ‰ƒCƒg‚Ìæ“¾
			AExtendedSpotLight* light = pLightMng->GetLight(i);
			if (IsValid(light) && m_shadows[i][n].IsValid()) {
				//‰e‚Éƒ‰ƒCƒg‚ğ“o˜^
				m_shadows[i][n].Get()->SetLight(light);
			}
		}
	}
}





//------------------------------------
// ‰eƒRƒ“ƒ|[ƒlƒ“ƒg‚ğ“o˜^
//------------------------------------
void UShadowManager::AddShadowComponent(UShadowComponent* _pShadowComponent)
{
	if (!IsValid(_pShadowComponent)) { return; }

	//lightIndex‚ª-1iBPã‚Å–¢İ’è‚È‚çˆ—‚µ‚È‚¢
	if (_pShadowComponent->GetLightIndex() < 0)
	{
		return;
	}

	int32 lightIndex = _pShadowComponent->GetLightIndex();
	//Œ»İ‚ÌTArray‚ÌÅ‘å—v‘f”Ô†‚Æƒ‰ƒCƒgƒCƒ“ƒfƒbƒNƒX‚Ì·
	int32 difference = (m_shadows.Num() - 1) - lightIndex;

	//“™‚µ‚¢–”‚ÍÅ‘å—v‘f”Ô†‚Ì•û‚ª‘å‚«‚¢‚È‚ç[lightIndex]‚Ì”z—ñ‚É’Ç‰Á
	if (difference == 0 || difference > 0)
	{
		m_shadows[lightIndex].Add(_pShadowComponent);
		return;
	}

	//Å‘å—v‘f”Ô†‚Ì•û‚ª¬‚³‚¢‚È‚ç
	if (difference < 0)
	{
		//ƒ‰ƒCƒgƒCƒ“ƒfƒbƒNƒX‚Ì”‚Ü‚ÅTArray‚ğ‘‚â‚µ
		for (int32 i = 0;i < abs(difference);++i)
		{
			m_shadows.Add(TArray<TWeakObjectPtr<UShadowComponent>>());
		}
		//ƒ‰ƒCƒgƒCƒ“ƒfƒbƒNƒX‚É’Ç‰Á
		m_shadows[lightIndex].Add(_pShadowComponent);
	}

}

//--------------------------------------------------------
// 
//--------------------------------------------------------
