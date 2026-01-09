// チェックポイント

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInstance/PlayDataInfo.h"
#include "Checkpoint.generated.h"

class UBoxComponent;

UCLASS()
class STEALTHACTION_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	int32 GetIndex() const;

	FCheckpointInfo GetCheckpointInfo()const { return m_checkpointInfo; }

	void SetHasCheckpoint(const bool _bHas);
	

	//プレイヤーとオーバーラップした瞬間のイベント関数
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void SetCheckPointInfo(const FCheckpointInfo& _info) { m_checkpointInfo = m_checkpointInfo; }

private:
	//メッシュコンポーネント
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* m_pMesh;
	//コリジョンコンポーネント
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* m_pCollisionBox;


	//チェックポイントの情報
	UPROPERTY(EditAnywhere, Category = "info")
	FCheckpointInfo m_checkpointInfo;

};
