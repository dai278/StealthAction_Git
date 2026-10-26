// Fill out your copyright notice in the Description page of Project Settings.

// インクルードガード
#pragma once

#include "CoreMinimal.h"
#include "MyGameInstance.h"

#include "GameFramework/Actor.h"
#include "GoalActor.generated.h"

//

//前方宣言
class UBoxComponent;
class UStaticMeshComponent;

class UWidgetComponent;


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

	//見た目のメッシュはBPで設定する
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	//UI
	UPROPERTY(EditAnywhere, Category = "UI")
	UWidgetComponent* GoalUI;
protected:
	//クリア画面のレベル名
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clear|Fade")
	FName ClearLevel = TEXT("GameClear");
	//クリア前の暗転時間
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clear|Fade", meta = (ClampMin = "0.0", Units = "s"))
	float FadeOutDuration = 0.5f;
	//暗転後に待つ時間
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clear|Fade", meta = (ClampMin = "0.0", Units = "s"))
	float BlackHoldDuration = 0.1f;
	//クリア画面で明るく戻す時間
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clear|Fade", meta = (ClampMin = "0.0", Units = "s"))
	float FadeInDuration = 0.5f;
	//フェードの色
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clear|Fade")
	FLinearColor FadeColor = FLinearColor::Black;

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
