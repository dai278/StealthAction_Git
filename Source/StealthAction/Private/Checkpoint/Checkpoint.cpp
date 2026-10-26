//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint/Checkpoint.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Checkpoint/CheckpointManager.h"


//----------------------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//----------------------------------------------------------
ACheckpoint::ACheckpoint()
	:m_checkpointInfo()
	, m_pMesh(nullptr)
	, m_pCollisionBox(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ƒƒbƒVƒ…ƒRƒ“ƒ|[ƒlƒ“ƒg‚Ì¶¬
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (m_pMesh)
	{
	}

	//ƒRƒŠƒWƒ‡ƒ“ƒRƒ“ƒ|[ƒlƒ“ƒg‚Ì¶¬
	m_pCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	if (m_pCollisionBox && m_pMesh)
	{
		m_pCollisionBox->SetupAttachment(m_pMesh);
		m_pCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnBeginOverlap);
	}
}


//----------------------------------------------------------
//  ƒQ[ƒ€ƒXƒ^[ƒgA‚Ü‚½‚Í¶¬‚ÉŒÄ‚Î‚ê‚é
//----------------------------------------------------------
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	//ƒ`ƒFƒbƒNƒ|ƒCƒ“ƒgƒ}ƒl[ƒWƒƒ[‚É“o˜^
	UCheckpointManager* checkpointManager = (GetWorld() ? GetWorld()->GetSubsystem<UCheckpointManager>() : nullptr);
	if (checkpointManager)
	{
		checkpointManager->AddCheckpoint(this);
	}

	m_checkpointInfo.Location = GetActorLocation();
	
	if (IsValid(checkpointManager) && m_checkpointInfo.Index == checkpointManager->GetCurrentCheckpointIndex())
	{
		m_checkpointInfo.bHasCheckpoint = false;
		if (IsValid(m_pCollisionBox)) { m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); }

	}
}

//----------------------------------------------------------
// –ˆƒtƒŒ[ƒ€‚ÌXVˆ—
//----------------------------------------------------------
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//----------------------------------------------------------
// ƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚ÌƒCƒ“ƒfƒbƒNƒX‚ğæ“¾‚·‚éŠÖ”
//----------------------------------------------------------
int32 ACheckpoint::GetIndex() const
{
	return m_checkpointInfo.Index;
}

//----------------------------------------------------------
/// ƒvƒŒƒCƒ„[‚ÆƒI[ƒo[ƒ‰ƒbƒv‚µ‚½uŠÔ‚ÌƒCƒxƒ“ƒgŠÖ”
//----------------------------------------------------------
void ACheckpoint::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{

	//ƒI[ƒo[ƒ‰ƒbƒv‚µ‚½ƒAƒNƒ^[‚ªƒvƒŒƒCƒ„[‚È‚ç
	//ƒ`ƒFƒbƒNƒ|ƒCƒ“ƒgƒ}ƒl[ƒWƒƒ[‚ÉŒ»İ‚Ìƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚ğİ’è‚·‚é
	//(ƒvƒŒƒCƒ„[”»’è‚ÍActor‚Ìƒ^ƒO‚Ås‚¤)
	if (OtherActor && OtherActor->ActorHasTag(FName("Player")))
	{
		UCheckpointManager* checkpointManager = (GetWorld() ? GetWorld()->GetSubsystem<UCheckpointManager>() : nullptr);
		if (checkpointManager)
		{
			checkpointManager->SetCurrentCheckpointIndex(m_checkpointInfo.Index);
			//ƒqƒbƒg‚ÌƒR[ƒ‹ƒoƒbƒNŠÖ”‚ÌŒÄ‚Ño‚µ
			checkpointManager->OnOverlapCheckpoint();
		}
	}
}

//---------------------------------------------------
//
//---------------------------------------------------
void ACheckpoint::SetHasCheckpoint(const bool _bHas)
{
	m_checkpointInfo.bHasCheckpoint = _bHas;
	if (!_bHas && IsValid(m_pCollisionBox))
	{
		m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}