//’S“–“úîà
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
	//ƒƒbƒVƒ…ƒRƒ“ƒ|[ƒlƒ“ƒg¶¬
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	m_pMesh->SetupAttachment(m_bodyCollision);
	//ƒRƒŠƒWƒ‡ƒ“ƒRƒ“ƒ|[ƒlƒ“ƒg¶¬
	m_pCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	m_pCollision->SetupAttachment(m_bodyCollision);
	// Overlap ‚ğ”­¶‚³‚¹‚é‚½‚ß‚É•K—v
	m_pCollision->SetGenerateOverlapEvents(true);


	Tags.Add(TEXT("Interact"));

	m_bodyCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	m_bodyCollision->SetCollisionObjectType(ECC_WorldDynamic);
	m_bodyCollision->SetCollisionResponseToAllChannels(ECR_Block);

	m_intractUI=CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	if (m_intractUI)
	{
		m_intractUI->SetupAttachment(RootComponent);
		m_intractUI->SetWidgetSpace(EWidgetSpace::Screen); // ‰æ–Ê‚ÉŒÅ’è‚·‚é‚È‚ç 
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

	//ŠJn‚É‚·‚Å‚É”ÍˆÍ“à‚Å‚à•\¦‚·‚éBƒvƒŒƒCƒ„[‚ªŒã‚©‚ç¶¬‚³‚ê‚Ä‚àæ‚è’¼‚·
	UpdateVisible();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(m_uiUpdateTimer, this, &AInteract::UpdateVisible, 0.1f, true);
	}
}

//-----------------------------------------------------
//I—¹ˆ—
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
//ƒCƒ“ƒ^ƒ‰ƒNƒgˆÊ’uæ“¾
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
//•\¦”ñ•\¦Ø‚è‘Ö‚¦XVˆ—
//-----------------------------------------------------
void AInteract::UpdateVisible()
{
	if (!IsValid(m_intractUI))
	{
		return;
	}

	//¶¬‡‚âƒŠƒXƒ|[ƒ“‚ÅQÆ‚ª•Ï‚í‚Á‚Ä‚àA¡‚ÌƒvƒŒƒCƒ„[‚ğg‚¤
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	m_pPlayer = Player;
	bool bVisible = false;
	if (IsValid(Player) && IsValid(m_pCollision) && CanInteract())
	{
		const double distance = FVector::Distance(GetActorLocation(), Player->GetActorLocation());
		if (distance <= m_visibleUILenght && !Player->IsInShadow())
		{
			//OverlapƒCƒxƒ“ƒg‚Ì‰ñ”‚Å‚Í‚È‚­A‘€ì”»’è‚Æ“¯‚¶Œ»İ‚Ì”ÍˆÍ‚ÅŒˆ‚ß‚é
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

	//ƒƒbƒVƒ…‚¾‚¯‚ª”ÍˆÍ‚ğ”²‚¯‚Ä‚àAƒJƒvƒZƒ‹‚ªc‚Á‚Ä‚¢‚ê‚ÎÁ‚³‚È‚¢
	if (Cast<APlayerCharacter>(OtherActor))
	{
		UpdateVisible();
	}
}


//---- ƒJƒƒ‰ƒtƒH[ƒJƒX—p‚Ìî•ñ‚ğ•Ô‚· -----
FCameraFocusData AInteract::GetCameraFocusData_Implementation() const
{

	return IsValid(FocusProviderComp) ? FocusProviderComp->GetCameraFocusData() : FCameraFocusData{};

}


//ƒJƒƒ‰ƒtƒH[ƒJƒXŠJnˆ—
void AInteract::CameraFocusStart(AActor* OtherActor)
{
	if (!IsValid(OtherActor) || !OtherActor->ActorHasTag(TEXT("Player"))) { return; }

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!IsValid(Player) || !IsValid(FocusProviderComp)) { return; }

	FCameraFocusData FocusData = FocusProviderComp->GetCameraFocusData();

	if (!IsValid(FocusData.FocusCamera)) { return; }

	Player->StartCameraFocus(this, FocusData.BlendTime);

}