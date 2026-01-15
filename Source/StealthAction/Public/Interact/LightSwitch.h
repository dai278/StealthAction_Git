// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/Interact.h"
#include "LightSwitch.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API ALightSwitch : public AInteract
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALightSwitch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	//派生先でインタラクトを上書き
	virtual void Interact();

	
public:
	UPROPERTY(EditAnywhere, Category = "Light")
	int32 m_lightIndex;
};
