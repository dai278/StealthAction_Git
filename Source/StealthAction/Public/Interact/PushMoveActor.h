// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/Interact.h"
#include "PushMoveActor.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API APushMoveActor : public AInteract
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APushMoveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	//インタラクトの実装
	virtual void Interact(AActor* _interactOwner) override;
	virtual void Interact(AActor* _interactOwner, bool& _isPose) override;

	//上移動
	void MoveUp(float DeltaTime);
	//下移動
	void MoveDown(float DeltaTime);
	//右移動
	void MoveRight(float DeltaTime);
	//左移動
	void MoveLeft(float DeltaTime);


	//インタラクトするポジションを取得
	virtual FVector GetInteractPosition(const AActor* _actor = nullptr)override;

	//移動リセット
	void ResetMove();

private:
	//移動距離
	UPROPERTY(EditAnywhere, Category = "PushMoveActor")
	float m_moveDistance = 200.f;

	//移動速度
	UPROPERTY(EditAnywhere, Category = "PushMoveActor")
	float m_moveSpeed;

	//移動中フラグ
	bool m_bIsMoving = false;

	//目標位置
	FVector m_targetLocation;

	//開始位置
	FVector m_startLocation;

	//初期位置
	FVector m_initPos;

	//移動進行度
	float m_moveProgress = 0.f;

	//移動方向
	FVector m_moveDirection;
	
	//移動処理
	
};
