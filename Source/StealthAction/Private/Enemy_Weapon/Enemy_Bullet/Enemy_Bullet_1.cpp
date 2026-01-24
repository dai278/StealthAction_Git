// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Weapon/Enemy_Bullet/Enemy_Bullet_1.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AEnemy_Bullet_1::AEnemy_Bullet_1()
    : m_bullletSpeed(8000.)
    , m_bullletRadius(25.)
    , m_bullletHeight(100.)
    , m_bullletDeleteTime(1.)
    , m_shakeVec(0.02)
{
    PrimaryActorTick.bCanEverTick = false;

    // Box 作成（当たり判定）
    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    // ルートに設定（重要）
    RootComponent = Box;

    // サイズ設定
    Box->InitBoxExtent(FVector(m_bullletHeight, m_bullletRadius, m_bullletRadius));

    //すり抜け専用 Collision
    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionObjectType(ECC_WorldDynamic);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    Box->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Bullet_1::OnBulletOverlap);

    // Mesh 作成
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

    //Mesh は当たり判定を持たせない
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->InitialSpeed = m_bullletSpeed;
    Movement->MaxSpeed = m_bullletSpeed;
    Movement->UpdatedComponent = Box;
    Movement->bAutoActivate = false;
    Movement->ProjectileGravityScale = 0.f;
    Movement->bSweepCollision = false;
}
void AEnemy_Bullet_1::ActivateBullet(const FVector& StartPos, const FVector& Direction, const FVector& UpDirection, const FVector& RightDirection)
{
    FVector randomVec[5] =
    {
        {Direction},
        {(Direction + UpDirection * m_shakeVec).GetSafeNormal() },
        {(Direction - UpDirection * m_shakeVec).GetSafeNormal() },
        {(Direction + RightDirection * m_shakeVec).GetSafeNormal() },
        {(Direction - RightDirection * m_shakeVec).GetSafeNormal() }
    };
    int32 Random = FMath::RandRange(0, 4);

    if (!Movement)
    {
        UE_LOG(LogTemp, Error, TEXT("ActivateBullet: Movement is NULL"));
        return;
    }
    GetWorldTimerManager().ClearTimer(LifeTimer);

    SetActorLocation(StartPos);
    SetActorRotation(randomVec[Random].Rotation());
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);



    Movement->Velocity = randomVec[Random] * m_bullletSpeed;
    Movement->Activate(true);

    bHidden = false;

    GetWorldTimerManager().SetTimer(LifeTimer, this, &AEnemy_Bullet_1::InvisibleBullet, m_bullletDeleteTime, false);
}

void AEnemy_Bullet_1::InvisibleBullet()
{

    if (Movement)
    {
        Movement->StopMovementImmediately();

        Movement->Deactivate();

    }

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    bHidden = true;

}

void AEnemy_Bullet_1::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
        return;

    if (OtherActor->ActorHasTag("Player"))
    {
        InvisibleBullet();
    }

    if (OtherActor->ActorHasTag("Wall"))
    {
        InvisibleBullet();
    }

    // ここでダメージ処理など
    // UGameplayStatics::ApplyDamage(...);

    // 弾を消してプールに戻す
}