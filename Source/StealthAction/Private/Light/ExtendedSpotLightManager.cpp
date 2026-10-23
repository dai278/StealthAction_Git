// Fill out your copyright notice in the Description page of Project Settings.


#include "Light/ExtendedSpotLightManager.h"
#include "Light/ExtendedSpotLight.h"

//----------------------------------------
//BeginPlay
//----------------------------------------
void UExtendedSpotLightManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	//ライト配列を空に
	m_pLights.Empty();
	//敵ライト配列を空に
	m_pEnemyLights.Empty();
}


//-------------------------------------------
//ライトの追加
//-------------------------------------------
void UExtendedSpotLightManager::AddLight(AExtendedSpotLight* _pLight)
{
	if (!IsValid(_pLight)) { return; }


	if (!_pLight->IsShadowDrawLight()) { 
		//-1なら敵ライト配列に追加
			m_pEnemyLights.AddUnique(_pLight);
		return; 
	}


	if (_pLight->GetIndex() < 0) { return; }

	//ライトを追加する要素数を特定
	for (int i = 0;i < m_pLights.Num(); ++i)
	{
		//この要素数のライトよりインデックスが低いなら
		//この要素数に追加する
		if (_pLight->GetIndex() ==i)
		{
			//nullなら追加
			if (!IsValid(m_pLights[i])) {
				//TArrayにライトを追加
				m_pLights[i]=_pLight;
				return;
			}
			
			//UE_LOG(LogTemp, Display, TEXT("LightのIndexがかぶっています"),i);
			return;
		}

	}

	//ここまで来たら現在の要素数より大きい為要素数をライトインデックスまで追加
	m_pLights.SetNum(_pLight->GetIndex()+1);
	m_pLights[_pLight->GetIndex()] = _pLight;
}


//ライト削除
void UExtendedSpotLightManager::UnRegisterLight(AExtendedSpotLight* _pLight)
{
	if (!_pLight) { return; }
	m_pEnemyLights.Remove(_pLight);
	for (AExtendedSpotLight*& Light : m_pLights)
	{
		if (Light == _pLight) { Light = nullptr; }
	}
}



//ライトの取得
AExtendedSpotLight* UExtendedSpotLightManager::GetLight(const int32 _lightIndex)
{
	//インデックスが有効範囲内ならそのインデックスのライトのアドレスを返す
	if (m_pLights.Num() > _lightIndex&&_lightIndex>=0) {
		
		return IsValid(m_pLights[_lightIndex]) ? m_pLights[_lightIndex] : nullptr;
	}
	else
	{
		//UE_LOG(LogTemp, Display, TEXT("無効なIndexのためnullを返します"));

		return nullptr;
	}
}


//---------------------
//ライトとの衝突チェック
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
//敵ライトとの衝突チェック
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
//影を落とすライトとに照らされている以下
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
