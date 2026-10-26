
#include "Objectives/ObjectivePoint.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/ObjectiveManager.h"

AObjectivePoint::AObjectivePoint()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent =
        CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    WidgetLocation =
        CreateDefaultSubobject<USceneComponent>(TEXT("WidgetLocation"));

    WidgetLocation->SetupAttachment(RootComponent);

    TriggerBox =
        CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));

    TriggerBox->SetupAttachment(RootComponent);

    // Trigger
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

    TriggerBox->SetGenerateOverlapEvents(true);

    bIsActive = false;
}


void AObjectivePoint::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(TriggerBox))
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(
            this,
            &AObjectivePoint::OnOverlap);
    }
}

void AObjectivePoint::OnOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    APawn* PlayerPawn =
        UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (IsValid(OtherActor) && IsValid(PlayerPawn) && OtherActor == PlayerPawn)
    {
        AObjectiveManager* Manager =
            Cast<AObjectiveManager>(
                UGameplayStatics::GetActorOfClass(
                    GetWorld(),
                    AObjectiveManager::StaticClass()));

        if (IsValid(Manager))
        {
            Manager->SetCurrentObjective(NextObjective);
        }
    }
}

FVector AObjectivePoint::GetWidgetWorldLocation() const
{
    if (IsValid(WidgetLocation))
    {
        return WidgetLocation->GetComponentLocation();
    }

    return GetActorLocation();
}