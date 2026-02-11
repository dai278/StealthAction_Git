// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OutLooKDoor.generated.h"



class UStaticMeshComponent;

UCLASS()
class STEALTHACTION_API AOutLooKDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOutLooKDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//閉める処理
	void CloseDoor();

	//チック1フレーム目にのみ呼ばれる処理
	void OnFrastTick();


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutLooKDoor")
	UStaticMeshComponent* m_doorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutLooKDoor")
	FVector m_closeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutLooKDoor")
	float m_closeSpeed;
		
};
