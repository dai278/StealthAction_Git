// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalActor.h"
#include "Engine/GameInstance.h"
#include "UI/Transition/LevelTransitionSubsystem.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"

//コンストラクタ
AGoalActor::AGoalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// ===== ルート作成 =====
	/*Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;*/

	// ===== コリジョン作成 =====
	//Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	//Collision->SetupAttachment(RootComponent);

	// ★ オーバーラップ専用設定 ★
	//Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Collision->SetCollisionResponseToAllChannels(ECR_Overlap);

	// ===== メッシュ作成 =====
	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//Mesh->SetupAttachment(RootComponent);
	//Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ルート作成
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// コリジョン作成
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);

	// メッシュ作成
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GoalUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("GoalUI"));
	GoalUI->SetupAttachment(RootComponent);
	GoalUI->SetWidgetSpace(EWidgetSpace::Screen); // 画面に固定するなら
	GoalUI->SetDrawSize(FVector2D(40, 40));
	

	// 初期状態で非表示にする場合はここで設定
	SetActorHiddenInGame(false);
}

//ゲームスタート時、または生成時に呼ばれる処理
void AGoalActor::BeginPlay()
{
	Super::BeginPlay();


	if (IsValid(Collision))
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AGoalActor::OnOverlapBegin);
	}
	if (IsValid(GoalUI)) { GoalUI->SetWidgetSpace(EWidgetSpace::Screen); }

}

// ===== ゴールに触れた =====
void AGoalActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IsValid(OtherActor)) return;

	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		if (ClearLevel.IsNone()) { return; }
		UGameInstance* Instance = GetGameInstance();
		ULevelTransitionSubsystem* Transition = Instance ? Instance->GetSubsystem<ULevelTransitionSubsystem>() : nullptr;
		if (!Transition || !Transition->OpenLevelWithFade(ClearLevel, FadeOutDuration, BlackHoldDuration, FadeInDuration, FadeColor))
		{
			UGameplayStatics::OpenLevel(this, ClearLevel);
		}
	}
}