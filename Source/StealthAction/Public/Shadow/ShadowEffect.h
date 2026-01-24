// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShadowEffect.generated.h"


enum class EPlayerStatus : uint8;
/**
 * 
 */
UCLASS()
class STEALTHACTION_API UShadowEffect : public UUserWidget
{
	GENERATED_BODY()

public:
	//コンストラクタ
	UShadowEffect(const FObjectInitializer& ObjectInitializer);

	//BeginPlay
	virtual void NativeOnInitialized() override;

	//プレイヤーの状態が変化したときに呼ばれる関数
	UFUNCTION()
	void OnPlayerStatusChanged(EPlayerStatus _state);
	
};
