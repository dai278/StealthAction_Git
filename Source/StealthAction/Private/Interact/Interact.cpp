// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/Interact.h"
#include "Components/BoxComponent.h"
#include "UI/IntactUIWidget.h"
#include "Components/WidgetComponent.h"

// Sets default values
AInteract::AInteract()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_bodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCollision"));

	SetRootComponent(m_bodyCollision);
	//メッシュコンポーネント生成
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	m_pMesh->SetupAttachment(m_bodyCollision);
	//コリジョンコンポーネント生成
	m_pCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	m_pCollision->SetupAttachment(m_bodyCollision);
	// Overlap を発生させるために必要
	m_pCollision->SetGenerateOverlapEvents(true);


	Tags.Add(TEXT("Interact"));

	m_bodyCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	m_bodyCollision->SetCollisionObjectType(ECC_WorldDynamic);
	m_bodyCollision->SetCollisionResponseToAllChannels(ECR_Block);

	m_intractUI=CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	if (m_intractUI)
	{
		m_intractUI->SetupAttachment(RootComponent);
		m_intractUI->SetWidgetSpace(EWidgetSpace::Screen); // 画面に固定するなら 
		m_intractUI->SetDrawSize(FVector2D(100, 100));
		m_intractUI->SetupAttachment(m_bodyCollision);
	}


}

// Called when the game starts or when spawned
void AInteract::BeginPlay()
{
	Super::BeginPlay();

	//UIは初期は非表示
	m_intractUI->SetVisibility(false);
	
	//コリジョンのオーバーラップイベント登録
	m_pCollision->OnComponentBeginOverlap.AddDynamic(this, &AInteract::OnOverlapBegin);
	m_pCollision->OnComponentEndOverlap.AddDynamic(this, &AInteract::OnOverlapEnd);
}

// Called every frame
void AInteract::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AInteract::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) 
{
	m_intractUI->SetVisibility(true);
}


void AInteract::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {

	m_intractUI->SetVisibility(false);
}