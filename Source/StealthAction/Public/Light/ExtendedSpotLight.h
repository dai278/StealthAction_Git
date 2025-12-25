// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExtendedSpotLight.generated.h"

//前方宣言
class USpotLightComponent;

UCLASS()
class STEALTHACTION_API AExtendedSpotLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExtendedSpotLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	//ライトインデックスの取得
	int32 GetIndex()const;

	//ライトの位置取得
	FVector GetLightLocation()const;

	//ライトの向き取得
	FVector GetLightForwardVector()const;

	//照射範囲
	//円錐の内側角度
	float GetLightInnerAngle()const;

	//円錐外側角度
	float GetLightOuterAngle()const;

	//光の届く距離
	float GetLightLength()const;

	//ライトが動くか?
	bool IsMove()const;

	//ライトのon,off設定
	UFUNCTION(BlueprintCallable)
	void SetVisibility(const bool isVisibility);

	//ライトとの衝突チェック
	bool IsHit(const FVector& _pos)const;

//	// 位置
//	FVector LightPos = SpotLight->GetComponentLocation();
//
//	// 向き
//	FVector LightDir = SpotLight->GetForwardVector();        // 正面方向
//	FVector ShadowDir = -SpotLight->GetForwardVector();      // 影の伸びる方向（逆方向）
//
//	// 照射範囲
//	float InnerAngle = SpotLight->InnerConeAngle;           // 円錐の内側角
//	float OuterAngle = SpotLight->OuterConeAngle;           // 円錐の外側角
//	float Range = SpotLight->AttenuationRadius;        // 光が届く最大距離

protected:
	
	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* m_pSpotLight;           //スポットライトコンポーネント

	UPROPERTY(EditAnywhere, Category = "Index")
	int32 m_lightIndex;   //ライトのインデックス

	UPROPERTY(EditAnywhere, Category = "Tick")
	bool m_bCanEverTick;                        //毎フレーム更新するか、Bpでも設定できるようにする

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* m_pMesh; //メッシュ	

	bool m_isEnemy;//所持者が敵か
};
