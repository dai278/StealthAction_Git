// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint/Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Checkpoint/CheckpointManager.h"


//----------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------
ACheckpoint::ACheckpoint()
	:m_checkpointInfo()
	, m_pMesh(nullptr)
	, m_pCollisionBox(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//メッシュコンポーネントの生成
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (m_pMesh)
	{
	}

	//コリジョンコンポーネントの生成
	m_pCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	if (m_pCollisionBox && m_pMesh)
	{
		m_pCollisionBox->SetupAttachment(m_pMesh);
		m_pCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnBeginOverlap);
	}
}


//----------------------------------------------------------
//  ゲームスタート時、または生成時に呼ばれる
//----------------------------------------------------------
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	//チェックポイントマネージャーに登録
	UCheckpointManager* checkpointManager = GetWorld()->GetSubsystem<UCheckpointManager>();
	if (checkpointManager)
	{
		checkpointManager->AddCheckpoint(this);
	}

	m_checkpointInfo.Location = GetActorLocation();
	
	if (m_checkpointInfo.Index == checkpointManager->GetCurrentCheckpointIndex())
	{
		m_checkpointInfo.bHasCheckpoint = false;
		m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
}

//----------------------------------------------------------
// 毎フレームの更新処理
//----------------------------------------------------------
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//----------------------------------------------------------
// チェックポイントのインデックスを取得する関数
//----------------------------------------------------------
int32 ACheckpoint::GetIndex() const
{
	return m_checkpointInfo.Index;
}

//----------------------------------------------------------
/// プレイヤーとオーバーラップした瞬間のイベント関数
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

	UE_LOG(LogTemp, Display, TEXT("checkpointHit"));
	//オーバーラップしたアクターがプレイヤーなら
	//チェックポイントマネージャーに現在のチェックポイントを設定する
	//(プレイヤー判定はActorのタグで行う)
	if (OtherActor && OtherActor->ActorHasTag(FName("Player")))
	{
		UCheckpointManager* checkpointManager = GetWorld()->GetSubsystem<UCheckpointManager>();
		if (checkpointManager)
		{
			checkpointManager->SetCurrentCheckpointIndex(m_checkpointInfo.Index);
			//ヒット時のコールバック関数の呼び出し
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
	if (!_bHas) { m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); }
}