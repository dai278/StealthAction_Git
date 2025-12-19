// Fill out your copyright notice in the Description page of Project Settings.

// インクルードガード
#pragma once

#include "CoreMinimal.h"
#include "MyGameInstance.h"

#include "GameFramework/Actor.h"
#include "GoalActor.generated.h"

//前方宣言
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class STEALTHACTION_API AGoalActor : public AActor
{
	GENERATED_BODY()
	
public:
	// コンストラクタ
	AGoalActor();

protected:
	// ゲーム開始時に呼ばれる
	virtual void BeginPlay() override;

public:
	// ===== コンポーネント =====

// ルート
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	// 当たり判定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* Collision;

	// 見た目（★ BP からメッシュ変更可能）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;
protected:
	// ===== オーバーラップイベント =====
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
