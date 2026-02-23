
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectivePoint.generated.h"

class UBoxComponent;

UCLASS()
class STEALTHACTION_API AObjectivePoint : public AActor
{
    GENERATED_BODY()

public:
    AObjectivePoint();

protected:
    virtual void BeginPlay() override;

public:

    // Widget表示位置
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* WidgetLocation;

    // TriggerBox
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* TriggerBox;

    // 次のObjective
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
    AObjectivePoint* NextObjective;

    // アクティブ状態
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool bIsActive;

    // Overlapイベント
    UFUNCTION()
    void OnOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    FVector GetWidgetWorldLocation() const;

};
