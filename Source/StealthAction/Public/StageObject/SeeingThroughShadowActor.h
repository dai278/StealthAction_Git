// Fill out your copyright notice in the Description page of Project Settings.
//------------------------------------
//透過機能、影描画機能を持つアクター
//--------------------------------------


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SeeingThroughShadowActor.generated.h"

//前方前言
class UStaticMeshComponent;
class UShadowComponent;

UCLASS()
class STEALTHACTION_API ASeeingThroughShadowActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// コンストラクタ
	ASeeingThroughShadowActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	static const float MinOpacity;// 最小不透明度
	static const float MaxOpacity;// 最大不透明度

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* m_pMesh; //透過させるスタティックメッシュ

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UShadowComponent* m_pShadow;  //影

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float m_seeingThroughtime;		// 透過処理する時間

	float m_timeCnt;	//透過時間カウントダウン
	float m_bSeeingThrough; //透過処理フラグ
};
