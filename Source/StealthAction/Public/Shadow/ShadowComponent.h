// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ShadowComponent.generated.h"

//前方前言
class AExtendedSpotLight;
class UBoxComponent;

//影の形状
UENUM(BlueprintType)
enum class EShadowShape :uint8
{
	Circle,
	box,
	Capsule,
	Num,
};

//スケールタイプ
UENUM(BlueprintType)
enum class EShadowScaleType:uint8
{
	OriginalScale	UMETA(DisplayName = "OriginalScale"),//オーナーのスケールを反映しない
	OwnerScaleX UMETA(DisplayName = "OwnerScaleX"),//Y軸の拡大を反映
	OwnerScaleY UMETA(DisplayName = "OwnerScaleY"),//X軸の拡大を反映
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHACTION_API UShadowComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UShadowComponent();

protected:
	// BeginPlay
	virtual void BeginPlay() override;


public:
	//更新処理
	//ELevelTick,FActorComponentTickFunctionは基本Supperに渡すくらいしか使わないらしい
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnRegister()override;

	


public:
	//ライトインデックスの取得
	int32 GetLightIndex()const;

	//ライトをセット
	void SetLight(const AExtendedSpotLight* _pLight);

	//位置や形などの更新
	void UpdateTransform();

private:

protected:

	UPROPERTY(VisibleAnywhere, Category = "Shadow/Mesh")
	UStaticMeshComponent* m_pShadowMesh;        	//メッシュコンポネント

	UPROPERTY(VisibleAnywhere, Category = "Shadow/Collision")
	UBoxComponent* m_pCollisionBox;	        //コリジョン

	UPROPERTY(EditAnywhere, Category = "Shadow/Index")
	int32 m_lightIndex;                     //自身を映すライトのインデックス

	UPROPERTY(EditAnywhere, Category = "Shadow/Transform")
	EShadowShape m_shadowShape;             //影の形状


	UPROPERTY(EditAnywhere, Category = "Shadow/Transform")
	float m_lengthScale;                    //影の長さ倍率

	UPROPERTY(EditAnywhere, Category = "Shadow/Transform")
	float m_width;							//幅の倍率

	UPROPERTY(EditAnywhere, Category = "Shadow/Transform")
	EShadowScaleType m_scaleType;        //スケールタイプ

	UPROPERTY(EditAnywhere, Category = "Shadow/Transform")
	bool  m_isUsingAbsoluteRotation; //回転をアタッチするか？



	float m_yaw;                            //影の向き

	float m_renderPos;						//描画する位置

	float m_length;                         //描画する長さ

	const AExtendedSpotLight* m_pLight;    //参照するライトのアドレス

	float m_collisionScaleZ;

	UPROPERTY(EditAnywhere, Category = "Shadow/C++")
	bool m_bAdjustShadow;
};
