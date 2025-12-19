// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

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

	// 初期状態で非表示にする場合はここで設定
	SetActorHiddenInGame(false);
}

//ゲームスタート時、または生成時に呼ばれる処理
void AGoalActor::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AGoalActor::OnOverlapBegin); 
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
	if (!OtherActor) return;

	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME CLEAR"));
		// クリア用レベルへ遷移
		UGameplayStatics::OpenLevel(this, FName("GameClear")); // レベル名を設定
	}
}