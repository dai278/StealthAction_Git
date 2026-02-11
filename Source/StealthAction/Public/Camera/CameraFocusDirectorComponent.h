// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraFocusProvider.h"

#include "CameraFocusDirectorComponent.generated.h"

class APlayerController;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHACTION_API UCameraFocusDirectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraFocusDirectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every fra
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
    // äJénèàóù
    //UFUNCTION(BlueprintCallable, Category = "Camera")
    void StartFocusByProviderActor(AActor* ProviderActor,bool& isStart,APlayerController* _controller);

    void StartFocus(AActor* ProviderActor, bool& isStart, APlayerController* _controller);


    // èIóπèàóù
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void ForceRestore();

private:
    void RestoreInternal(bool bUnpause);

private:
    TWeakObjectPtr<AActor> SavedViewTarget;
    bool bFocusing = false;
    bool bPausedGame = false;

    float SavedBlendOutTime = 0.3f;
    double EndRealTime = 0.0;
};
