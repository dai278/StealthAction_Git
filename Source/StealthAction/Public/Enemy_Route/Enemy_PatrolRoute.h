// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy_PatrolRoute.generated.h"

UCLASS()
class STEALTHACTION_API AEnemy_PatrolRoute : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemy_PatrolRoute();

	//„‰ñŠÖŒW
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Route")
	int m_route_Num;		//ƒ‹[ƒg”Ô†‚ğŒˆ‚ß‚é•Ï”

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Route")
	int m_route_ActionOrder;		//ˆÚ“®‡‚ğŒˆ‚ß‚é•Ï”


};
