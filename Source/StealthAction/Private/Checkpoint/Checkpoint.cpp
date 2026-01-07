// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint/Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Checkpoint/CheckpointManager.h"

//----------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------
ACheckpoint::ACheckpoint()
	:m_index(0)
	, m_pMesh(nullptr)
	, m_pCollisionBox(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//メッシュコンポーネントの生成
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	//コリジョンコンポーネントの生成
	m_pCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));

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
	return m_index;
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
	//オーバーラップしたアクターがプレイヤーなら
	//チェックポイントマネージャーに現在のチェックポイントを設定する
	//(プレイヤー判定はActorのタグで行う)
	if (OtherActor && OtherActor->ActorHasTag(FName("Player")))
	{
		UCheckpointManager* checkpointManager = GetWorld()->GetSubsystem<UCheckpointManager>();
		if (checkpointManager)
		{
			checkpointManager->SetCurrentCheckpointIndex(m_index);
		}
	}
}

