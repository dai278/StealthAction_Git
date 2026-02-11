// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "KeyItemSubsystem.generated.h"

class AItemKey;

/**
* 
 * 
 */
UCLASS()
class STEALTHACTION_API UKeyItemSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	//ƒxƒMƒ“ƒvƒŒƒC
	void OnWorldBeginPlay(UWorld& InWorld) override;
	
	void RegisterKeyItem(AItemKey* _keyItem);
	
	
	AItemKey* GetKeyItem() ;

private:
private:
	UPROPERTY()
	TObjectPtr<AItemKey> keyItem;
};
