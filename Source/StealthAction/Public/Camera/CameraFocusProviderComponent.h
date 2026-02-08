// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraFocusProvider.h"
#include "Camera/CameraActor.h"
#include "CameraFocusProviderComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHACTION_API UCameraFocusProviderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraFocusProviderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FCameraFocusData CameraFocusData;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	FCameraFocusData GetCameraFocusData() const { return CameraFocusData; };
};
