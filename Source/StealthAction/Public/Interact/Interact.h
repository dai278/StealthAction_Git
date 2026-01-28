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

public:
	//インデックス取得
	virtual int32 GetIndex()const { return m_index; }
protected:
	UPROPERTY(EditAnywhere, Category = "Collision")
	UBoxComponent* m_pCollision;			//コリジョンコンポネント

	UPROPERTY(VisibleAnywhere, Category = "Shadow/Mesh")
	UStaticMeshComponent* m_pMesh;        	//メッシュコンポネント

	UBoxComponent* m_bodyCollision;//壁判定用コリジョン

	UPROPERTY(EditAnywhere, Category = "Interact")
	int32 m_index; 							//インタラクト識別用インデックス

};
