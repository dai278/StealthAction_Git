//-----------------------------------------
// プレイ中発生した音を管理するクラス
//-----------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NoiseManager.generated.h"

//前方宣言
class UNoiseListenerComponent;
/**
 * 
 */
UCLASS()
class STEALTHACTION_API UNoiseManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//BeginPlay
	virtual void OnWorldBeginPlay(UWorld&)override;

	//音を鳴らす
	void MakeNoise(const int& _noiseVolume, const FVector& _pos);

	//リスナーの追加
	void AddListener(UNoiseListenerComponent* _listener);

private:
	TArray<UNoiseListenerComponent*> m_noiseListeners;
};
