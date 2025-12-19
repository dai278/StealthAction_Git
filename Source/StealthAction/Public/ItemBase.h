// Fill out your copyright notice in the Description page of Project Settings.

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

//前方宣言
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class STEALTHACTION_API AItemBase : public AActor
{
	//UEのオブジェクトクラスで必ず先頭に書くマクロ
	GENERATED_BODY()

public:
	//コンストラクタ
	AItemBase();

protected:
	//ゲームスタート時、または生成時に呼ばれる処理
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	virtual void HandleOverlap(AActor* OtherActor);

protected:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
};
