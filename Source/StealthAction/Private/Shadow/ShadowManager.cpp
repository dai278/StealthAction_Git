// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "Shadow\ShadowManager.h"
#include "Shadow/ShadowComponent.h"
#include "Light/ExtendedSpotLightManager.h"
#include "Light//ExtendedSpotLight.h"

//-------------------------------------
//コンストラクタ
//-------------------------------------
UShadowManager::UShadowManager()
{

}


//----------------------------------------
//BeginPlay
//----------------------------------------
void UShadowManager::OnWorldBeginPlay(UWorld&)
{
	//配列を空にして置く
	for (int i = 0;i < m_shadows.Num();++i)
	{
		m_shadows[i].Empty();
	}
	m_shadows.Empty();

	//位置フレーム後に指定の関数を呼び出す処理
	//ライトがライトマネージャーに登録が終わった後に影にライトを登録したいが、
	//BeginPlayの呼び出し順がSubSystem→Actorな為
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UShadowManager::AllLinkLights);

}


//-------------------------------------------
//ライトを影に設定する
// Tickの１フレーム目にのみ呼ばれる
//------------------------------------------
void UShadowManager::AllLinkLights()
{
	//ライトマネージャーのアドレス
	UExtendedSpotLightManager* pLightMng = GetWorld()->GetSubsystem<UExtendedSpotLightManager>();

	//nullチェック
	if (!pLightMng) { return; }

	for (int i = 0;i < m_shadows.Num();++i)
	{
		for (int n = 0;n < m_shadows[i].Num();++n)
		{
			//ライトの取得
			AExtendedSpotLight* light = pLightMng->GetLight(i);
			if (light) {
				//影にライトを登録
				m_shadows[i][n]->SetLight(light);
			}
		}
	}
}





//------------------------------------
// 影コンポーネントを登録
//------------------------------------
void UShadowManager::AddShadowComponent(UShadowComponent* _pShadowComponent)
{
	//lightIndexが-1（BP上で未設定なら処理しない
	if (_pShadowComponent->GetLightIndex() == -1)
	{
		UE_LOG(LogTemp, Display, TEXT("ライトIndexが設定されていません：ShadowManager:AddShadowComponent"));
		return;
	}

	int32 lightIndex = _pShadowComponent->GetLightIndex();
	//現在のTArrayの最大要素番号とライトインデックスの差
	int32 difference = (m_shadows.Num() - 1) - lightIndex;

	//等しい又は最大要素番号の方が大きいなら[lightIndex]の配列に追加
	if (difference == 0 || difference > 0)
	{
		m_shadows[lightIndex].Add(_pShadowComponent);
		return;
	}

	//最大要素番号の方が小さいなら
	if (difference < 0)
	{
		//ライトインデックスの数までTArrayを増やし
		for (int32 i = 0;i < abs(difference);++i)
		{
			m_shadows.Add(TArray<UShadowComponent*>());
		}
		//ライトインデックスに追加
		m_shadows[lightIndex].Add(_pShadowComponent);
	}

}

//--------------------------------------------------------
// 
//--------------------------------------------------------
