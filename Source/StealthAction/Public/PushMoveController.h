// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/Interact.h"
#include "PushMoveController.generated.h"

class APushMoveActor;
/**
 * 
 */
UCLASS()
class STEALTHACTION_API APushMoveController : public AInteract
{
	GENERATED_BODY()

public:
	APushMoveController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AActor* _interactOwner) override;

	virtual FCameraFocusData GetCameraFocusData_Implementation() const override;

	//カメラフォーカススタート
	//void CameraFocusStart(AActor* OtherActor) override;
private:

	UPROPERTY(EditAnywhere, Category = "PushMove")
	TObjectPtr<APushMoveActor> m_pushMoveActors;	//移動させるアクターの配列	


	bool m_bInteractStart;	//インタラクト開始したかどうか

	bool m_isCameraFocusEnd;	//カメラフォーカス終了したかどうか

	bool m_isPlayerInput;	//プレイヤーの入力を受け付けるかどうか
};
