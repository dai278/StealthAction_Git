// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interact.generated.h"

class UBoxComponent;
class UWidgetComponent;


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

    //インタラクトするポジションを取得
    virtual FVector GetInteractPosition(const AActor* _actor = nullptr);

    //表示非表示切り替え更新
    virtual void UpdateVisible();
public:
    //インデックス取得
    virtual int32 GetIndex()const { return m_index; }

protected:
	//衝突開始イベント関数
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
    
	////衝突終了イベント関数
 //   UFUNCTION()
 //   void OnOverlapEnd(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
 //       
protected:
    UPROPERTY(EditAnywhere, Category = "Collision")
    UBoxComponent* m_pCollision;			//コリジョンコンポネント

    UPROPERTY(VisibleAnywhere, Category = "Shadow/Mesh")
    UStaticMeshComponent* m_pMesh;        	//メッシュコンポネント

    UBoxComponent* m_bodyCollision;//壁判定用コリジョン

    UPROPERTY(EditAnywhere, Category = "Interact")
    int32 m_index; 							//インタラクト識別用インデックス

    UPROPERTY(EditAnywhere, Category = "UI")
    UWidgetComponent* m_intractUI;

    UPROPERTY(EditAnywhere, Category = "Interact")
    TArray<FVector> InteractPos;

    //プレイヤー参照用
    APawn* m_pPlayer;

    UPROPERTY(EditAnywhere, Category = "Interact")
    float m_visibleUILenght;

};
