// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ExtendedSpotLightManager.generated.h"

//前方宣言
class AExtendedSpotLight;

/**
 * 
 */
UCLASS()
class STEALTHACTION_API UExtendedSpotLightManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	//BeginPlay
	virtual void OnWorldBeginPlay(UWorld&)override;

	//ライトの追加
	void AddLight(AExtendedSpotLight* _pLight);

	//ライトの取得
	AExtendedSpotLight* GetLight(const int32 _lightIndex);

	//lightの衝突チェック
	bool IsHitAllLight(const FVector& _pos)const;
	bool IsHitEnemyLight(const FVector& _pos)const;
	bool IsHitShadowDrawLight(const FVector& _pos)const;
	
	void SwitchTurnLight(const int32& _index);
protected:
	//レベル上のライトを管理する
	TArray <AExtendedSpotLight*> m_pLights;
	//敵のライト
	TArray <AExtendedSpotLight*> m_pEnemyLights;


	
};
