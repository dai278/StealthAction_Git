// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/Interact.h"
#include "PushMoveReset.generated.h"

class APushMoveActor;

/**
 * 
 */
UCLASS()
class STEALTHACTION_API APushMoveReset : public AInteract
{
	GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APushMoveReset();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    //派生先でインタラクトを上書き
    virtual void Interact(AActor* _interactOwner)override;

	UPROPERTY(EditAnywhere, Category = "Interact")
    TArray< TObjectPtr<APushMoveActor>> m_resetActor;
	
};
