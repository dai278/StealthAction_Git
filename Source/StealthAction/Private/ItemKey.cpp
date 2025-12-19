// Fill out your copyright notice in the Description page of Project Settings.


//インクルード
#include "ItemKey.h"
#include "GoalActor.h"
#include "Kismet/GameplayStatics.h"
//コンストラクタ
AItemKey::AItemKey()
{
	GoalSpawnLocation = FVector(0.f, 0.f, 0.f);
}

void AItemKey::HandleOverlap(AActor* OtherActor)
{
	if (!OtherActor) return;

	UE_LOG(LogTemp, Warning, TEXT("[]HandleOverlap called: %s"), *OtherActor->GetName());

	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		UE_LOG(LogTemp, Warning, TEXT("[]KEY GET"));

		// ゴール生成
        if (GoalClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("[]Spawning Goal at X=%.0f Y=%.0f Z=%.0f"),
                GoalSpawnLocation.X, GoalSpawnLocation.Y, GoalSpawnLocation.Z);

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;

            AGoalActor* SpawnedGoal = GetWorld()->SpawnActor<AGoalActor>(
                GoalClass,
                GoalSpawnLocation,
                FRotator::ZeroRotator,
                SpawnParams
            );

            if (SpawnedGoal)
            {
                SpawnedGoal->SetActorHiddenInGame(false);
                SpawnedGoal->SetActorEnableCollision(true);
                if (SpawnedGoal->Mesh)
                {
                    SpawnedGoal->Mesh->SetHiddenInGame(false);
                }

                UE_LOG(LogTemp, Warning, TEXT("[]Goal is now visible"));
            }
        }
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[]GoalClass is NULL"));
		}

		Destroy();
	}
}