#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Enemy_Bullet_1.generated.h"

class UProjectileMovementComponent;

UCLASS()
class STEALTHACTION_API AEnemy_Bullet_1 : public AActor
{
    GENERATED_BODY()

public:
    AEnemy_Bullet_1();

    void ActivateBullet(const FVector& StartPos, const FVector& Direction);
    void InvisibleBullet();
    bool IsHidden() const { return bHidden; }

    TArray<AEnemy_Bullet_1*> m_pEnemy_Bullet_1;	//その他のエネミー_1ポインタ
    bool bHidden = true;
private:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* Movement;

    UFUNCTION()
    void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int OtherBodyIndex,bool bFromSweep,const FHitResult&SweepResult);



protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UBoxComponent* Box;
public:
    UPROPERTY(EditAnywhere, Category = "BullletSpeed")
    float m_bullletSpeed;			//弾速

    UPROPERTY(EditAnywhere, Category = "BullletSize")
    float m_bullletRadius;			//半径
    UPROPERTY(EditAnywhere, Category = "BullletSize")
    float m_bullletHeight;			//高さ
    
    UPROPERTY(EditAnywhere, Category = "BullletTime")
    float m_bullletDeleteTime;			//弾の消滅時間

    FTimerHandle LifeTimer;


};