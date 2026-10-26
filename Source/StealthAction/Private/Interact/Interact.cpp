// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/Interact.h"
#include "Components/BoxComponent.h"
#include "UI/IntactUIWidget.h"
#include "Kismet/GameplayStatics.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"

// Sets default values
AInteract::AInteract()
	:m_pPlayer(nullptr)
	, m_visibleUILenght(1000.f)
	, m_IsUseInteract(true)
	, FocusProviderComp(nullptr)
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

	InteractPos.Add(FVector::ZeroVector);

	m_indexs.Empty();

	m_indexs.Add(0);
}

// Called when the game starts or when spawned
void AInteract::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(m_intractUI))
	{
		m_intractUI->SetVisibility(false);
	}
	if (!IsValid(m_pCollision))
	{
		return;
	}

	m_pCollision->SetCollisionProfileName(FName("Interact"));
	m_pCollision->OnComponentBeginOverlap.AddDynamic(this, &AInteract::OnOverlapBegin);
	m_pCollision->OnComponentEndOverlap.AddDynamic(this, &AInteract::OnOverlapEnd);

	//開始時にすでに範囲内でも表示する。プレイヤーが後から生成されても取り直す
	UpdateVisible();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(m_uiUpdateTimer, this, &AInteract::UpdateVisible, 0.1f, true);
	}
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void AInteract::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(m_uiUpdateTimer);
	}
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AInteract::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


//-----------------------------------------------------
//インタラクト位置取得
//-----------------------------------------------------
FVector AInteract::GetInteractPosition(const AActor* _actor)
{
	if(InteractPos.Num()>0)
	{
		return InteractPos[0];
	}
	return GetActorLocation();
}


//-----------------------------------------------------
//表示非表示切り替え更新処理
//-----------------------------------------------------
void AInteract::UpdateVisible()
{
	if (!IsValid(m_intractUI))
	{
		return;
	}

	//生成順やリスポーンで参照が変わっても、今のプレイヤーを使う
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	m_pPlayer = Player;
	bool bVisible = false;
	if (IsValid(Player) && IsValid(m_pCollision) && CanInteract())
	{
		const double distance = FVector::Distance(GetActorLocation(), Player->GetActorLocation());
		if (distance <= m_visibleUILenght && !Player->IsInShadow())
		{
			//Overlapイベントの回数ではなく、操作判定と同じ現在の範囲で決める
			bVisible = Player->IsInInteractRange(this);
		}
	}

	if (m_intractUI->IsVisible() != bVisible)
	{
		m_intractUI->SetVisibility(bVisible);
	}
}

void AInteract::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) 
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		UpdateVisible();
	}
}


void AInteract::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {

	//メッシュだけが範囲を抜けても、カプセルが残っていれば消さない
	if (Cast<APlayerCharacter>(OtherActor))
	{
		UpdateVisible();
	}
}


//---- カメラフォーカス用の情報を返す -----
FCameraFocusData AInteract::GetCameraFocusData_Implementation() const
{

	return IsValid(FocusProviderComp) ? FocusProviderComp->GetCameraFocusData() : FCameraFocusData{};

}


//カメラフォーカス開始処理
void AInteract::CameraFocusStart(AActor* OtherActor)
{
	if (!IsValid(OtherActor) || !OtherActor->ActorHasTag(TEXT("Player"))) { return; }

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!IsValid(Player) || !IsValid(FocusProviderComp)) { return; }

	FCameraFocusData FocusData = FocusProviderComp->GetCameraFocusData();

	if (!IsValid(FocusData.FocusCamera)) { return; }

	Player->StartCameraFocus(this, FocusData.BlendTime);

}