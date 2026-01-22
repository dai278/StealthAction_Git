// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interact.generated.h"

class UBoxComponent;


UCLASS()
class STEALTHACTION_API AInteract : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteract();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	//派生先でインタラクトを上書き
	virtual void Interact(AActor* _interactOwner) {};

protected:
	UPROPERTY(EditAnywhere, Category = "Collision")
	UBoxComponent* m_pCollision;

	UPROPERTY(VisibleAnywhere, Category = "Shadow/Mesh")
	UStaticMeshComponent* m_pMesh;        	//メッシュコンポネント


};
