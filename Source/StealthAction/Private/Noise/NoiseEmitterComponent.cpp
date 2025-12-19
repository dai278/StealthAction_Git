// Fill out your copyright notice in the Description page of Project Settings.


#include "Noise/NoiseEmitterComponent.h"
#include "Noise/NoiseManager.h"

// Sets default values for this component's properties
UNoiseEmitterComponent::UNoiseEmitterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UNoiseEmitterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNoiseEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//‰¹‚ð–Â‚ç‚·
void UNoiseEmitterComponent::MakeNoise(const int& _volume, const FVector& _pos)
{

	UNoiseManager* manager = GetWorld()->GetSubsystem<UNoiseManager>();
	if (!manager) { return; }

	manager->MakeNoise(_volume, _pos);
}