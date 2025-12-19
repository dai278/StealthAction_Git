// Fill out your copyright notice in the Description page of Project Settings.


//インクルード
#include "ItemBase.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

//コンストラクタ
AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// ★ BoxをRootにする（重要）
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = Collision;

	// Overlap専用設定
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// Meshは当たり判定しない
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

//ゲームスタート時、または生成時に呼ばれる処理
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AItemBase::OnOverlapBegin
	);

	UE_LOG(LogTemp, Warning, TEXT("Item Spawned"));
}


//オーバーラップ接触し始めた時の処理
void AItemBase::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	UE_LOG(LogTemp, Warning, TEXT("[]ItemBase Overlap Begin"));

	HandleOverlap(OtherActor);
}

void AItemBase::HandleOverlap(AActor* OtherActor)
{
	// 基本アイテムは何もしない
}
