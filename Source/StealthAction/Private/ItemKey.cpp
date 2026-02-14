// Fill out your copyright notice in the Description page of Project Settings.


//インクルード
#include "ItemKey.h"
#include "GoalActor.h"
#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "UI/HUD/HUDWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Components/WidgetComponent.h"

#include "KeyItemSubsystem.h"
//コンストラクタ
AItemKey::AItemKey()
{
	GoalSpawnLocation = FVector(0.f, 0.f, 0.f);

    KeyUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("GoalUI"));
    KeyUI->SetupAttachment(RootComponent);
    KeyUI->SetWidgetSpace(EWidgetSpace::Screen); // 画面に固定するなら
    KeyUI->SetDrawSize(FVector2D(40, 40));


}

//ゲームスタート時、または生成時に呼ばれる処理
void AItemKey::BeginPlay()
{
    Super::BeginPlay();
    
    // キーアイテムをワールドサブシステムに登録
    UKeyItemSubsystem* KeyItemSubsystem = GetWorld()->GetSubsystem<UKeyItemSubsystem>();
    if (KeyItemSubsystem)
    {
        KeyItemSubsystem->RegisterKeyItem(this);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[]Failed to get KeyItemSubsystem"));
    }

	// 初期状態で非表示にする場合はここで設定
	KeyUI->SetVisibility(true);
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

			// キーアイテムUI表示
            KeyUI->SetVisibility(false);

			//カメラフォーカス開始
			CameraFocusStart(OtherActor);

			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
            if (PlayerCharacter)
            {
                PlayerCharacter->OnGetKeyItem();
            }

			// キーアイテム取得コールバックの呼び出し
			if (m_onGetKeyItemCallback) { m_onGetKeyItemCallback(); }
        }
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[]GoalClass is NULL"));
		}

		Destroy();
	}
}