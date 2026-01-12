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

public:

	UFUNCTION()
	void TurnOn();//付ける

	UFUNCTION()
	void TurnOff();//消す

	UFUNCTION()
	bool IsCanItBeTurned() { return m_canItBeTurned; }

private:
	//Yaw回転の更新処理
	void UpdateYawRotate(const float& _deltaTime);
	//Pitch回転
	void UpdatePitchRotate(const float& _deltaTime);

	//点滅の更新処理
	void UpdateBlink(const float& _deltaTime);
protected:
	UPROPERTY(EditAnywhere, Category = "Rotate")
	bool m_bAutomaticRotateYaw;//Yaw方向に自動回転するか

	UPROPERTY(EditAnywhere, Category = "Rotate")
	float m_automaticRotateYawSpeed;//Yaw方向回転スピード

	UPROPERTY(EditAnywhere, Category = "Rotate")
	bool m_bRotateTurn;                  //回転中ターンするか

	
	UPROPERTY(EditAnywhere, Category = "Rotate")
	float m_minTurnRotate; //ターンする角度

	UPROPERTY(EditAnywhere, Category = "Rotate")
	float m_maxTurnRotate;//ターンする最大角度

	UPROPERTY(EditAnywhere, Category = "Rotate")
	int32 m_turnDir;


	UPROPERTY(EditAnywhere, Category = "Rotate")
	bool m_bAutomaticRotatePitch;//Yaw方向に自動回転するか

	UPROPERTY(EditAnywhere, Category = "Rotate")
	float m_automaticRotatePitchSpeed;//Yaw方向回転スピード


	UPROPERTY(EditAnywhere, Category = "blink")
	bool m_bBlink;     //点滅するか

	UPROPERTY(EditAnywhere, Category = "blink")
	float m_blinkOnTime;//ついているじかん

	UPROPERTY(EditAnywhere, Category = "blink")
	float m_blinkOffTime;//消えている時間

	UPROPERTY(EditAnywhere, Category = "blink")
	bool m_bIsOn;//ついているか
	
	float m_blinkTimer;//経過時間計測用タイマー

	UPROPERTY(EditAnywhere, Category = "blink")
	bool m_canItBeTurned;//ONOFFの操作可能か
};
