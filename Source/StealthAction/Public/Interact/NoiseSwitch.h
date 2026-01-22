// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/Interact.h"
#include "NoiseSwitch.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API ANoiseSwitch : public AInteract
{
	GENERATED_BODY()

public:
	// コンストラクタ
	ANoiseSwitch();
protected:
	// ベギンプレイ
	virtual void BeginPlay() override;
public:
	
	// 更新
	virtual void Tick(float DeltaTime) override;
	
	//インタラクト
	virtual void Interact(AActor* _interactOwner) override;

private:
	bool m_bIsOn;// スイッチの状態

	UPROPERTY(EditAnywhere, Category = "NoiseSwitch")
	TArray<int32> m_makeNoiseObjectIndex;// 騒音を出すオブジェクトのインデックス	
};
