// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ShadowManager.generated.h"

/**
 * 
 */
 // 前方前言
class UShadowComponent;
class UShadowTraversalComponent;

UCLASS()
class STEALTHACTION_API UShadowManager : public UWorldSubsystem
{

	GENERATED_BODY()

public:

	//BeginPlay
	virtual void OnWorldBeginPlay(UWorld&)override;

	//コンストラクタ
	UShadowManager();

public:

	//ライトを影に設定する
	void AllLinkLights();

	// 影コンポーネントを追加
	void AddShadowComponent(UShadowComponent* _pShadowComponent);

private:
	//影を管理する二重配列ライトインデックスごとに分ける
	TArray<TArray<UShadowComponent*>> m_shadows;
};