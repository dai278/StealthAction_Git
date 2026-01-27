// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthAction/PlayerCharacter/MyCharacterMovementComponent.h"


void UMyCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
	if (IsCrouching())
	{
		MaxWalkSpeed = 200.0f;
	}
	else
	{
		MaxWalkSpeed = 600.0f;
	}
}