// Enemy_Effect_X.cpp

#include "Enemy_Effect/Enemy_Effect_1.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"

AEnemy_Effect_1::AEnemy_Effect_1()
    : m_effectDeleteTime(3.f)
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
    Billboard->SetupAttachment(RootComponent);
    Billboard->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Billboard->bIsScreenSizeScaled = true;

    // 初期は必ず非表示（プール前提）
    SetActorHiddenInGame(true);
}

void AEnemy_Effect_1::ActivateEffect(const FVector& StartPos, AActor* TargetActor)
{
    //UE_LOG(LogTemp, Warning, TEXT("EffectActiveX"));

    // 以前のタイマーが残っていると再利用時に事故る
    GetWorldTimerManager().ClearTimer(DestroyTimer);

    // 位置
    SetActorLocation(StartPos);

    // Attach は安全チェック
    if (IsValid(TargetActor))
    {
        AttachToActor(TargetActor, FAttachmentTransformRules::KeepWorldTransform);
    }
    else
    {
        // ターゲットなしならアタッチしない（ワールドに置くだけ）
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    }

    // 表示（bHiddenを触らない）
    SetActorHiddenInGame(false);
    if (Billboard)
    {
        // Actor Hiddenだけで足りるならこの2行は削っても良い
        Billboard->SetHiddenInGame(false);
        Billboard->SetVisibility(true, true);
    }

    // タイマー
    UWorld* World = GetWorld();
    if (!World)
    {
        // World無しは安全に戻す
        InvisibleEffect();
        return;
    }

    World->GetTimerManager().SetTimer(
        DestroyTimer,
        this,
        &AEnemy_Effect_1::InvisibleEffect,
        m_effectDeleteTime,
        false
    );
}

void AEnemy_Effect_1::InvisibleEffect()
{
    // 再入対策：まずタイマーを止める
    GetWorldTimerManager().ClearTimer(DestroyTimer);

    // Detach
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // 非表示
    SetActorHiddenInGame(true);

    if (Billboard)
    {
        Billboard->SetVisibility(false, true);
        Billboard->SetHiddenInGame(true);
    }
}