// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/Interact.h"
#include "LightTurnSwitch.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API ALightTurnSwitch : public AInteract
{
	GENERATED_BODY()

public:
	ALightTurnSwitch();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	//派生先でインタラクトを上書き
	virtual void Interact(AActor* _interactOwner)override;

};
