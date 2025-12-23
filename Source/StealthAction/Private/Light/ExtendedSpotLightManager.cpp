// Fill out your copyright notice in the Description page of Project Settings.


#include "Light/ExtendedSpotLightManager.h"
#include "Light/ExtendedSpotLight.h"

//----------------------------------------
//BeginPlay
//----------------------------------------
void UExtendedSpotLightManager::OnWorldBeginPlay(UWorld&)
{
	//Super::OnWorldBeginPlay();
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

	if (_pLight->GetIndex() == -1) { 
		//-1なら敵ライト配列に追加
			m_pEnemyLights.Add(_pLight);
		return; 
	}


	//ライトを追加する要素数を特定
	for (int i = 0;i < m_pLights.Num(); ++i)
	{
		//この要素数のライトよりインデックスが低いなら
		//この要素数に追加する
		if (_pLight->GetIndex() ==i)
		{
			//nullなら追加
			if (m_pLights[i] == nullptr) {
				//TArrayにライトを追加
				m_pLights[i]=_pLight;
				return;
			}
			
			UE_LOG(LogTemp, Display, TEXT("LightのIndexがかぶっています"),i);
			return;
		}

	}

	//ここまで来たら現在の要素数より大きい為要素数をライトインデックスまで追加
	m_pLights.SetNum(_pLight->GetIndex()+1);
	m_pLights[_pLight->GetIndex()] = _pLight;
}

//ライトの取得
AExtendedSpotLight* UExtendedSpotLightManager::GetLight(const int32 _lightIndex)
{
	//インデックスが有効範囲内ならそのインデックスのライトのアドレスを返す
	if (m_pLights.Num() > _lightIndex&&_lightIndex>=0) {
		
		return m_pLights[_lightIndex];
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("無効なIndexのためnullを返します"));

		return nullptr;
	}
}


//---------------------
//ライトとの衝突チェック
//-------------------
bool UExtendedSpotLightManager::IsHitLight(const FVector& _pos)const
{
	for (int i = 0; i < m_pLights.Num(); ++i)
	{
		if (m_pLights[i]->IsHit(_pos))
		{
			return true;
		}
	}
	for(int i = 0; i < m_pEnemyLights.Num(); ++i)
	{
		if (m_pEnemyLights[i]->IsHit(_pos))
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
		if (m_pEnemyLights[i]->IsHit(_pos))
		{
			return true;
		}
	}
	return false;
}
