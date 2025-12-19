// -------------------------------------
//　音を感知するコンポーネント
// -------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoiseListenerComponent.generated.h"

//クラスのメンバ関数をノイズイベントのコールバックにするときのマクロ
#define CreateNoiseEventCallback(_func) std::bind(&_func, this, std::placeholders::_1, std::placeholders::_2)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHACTION_API UNoiseListenerComponent : public UActorComponent
{
	

	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNoiseListenerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//コールバック関数の登録
	void SetCallBackFunction(const std::function<void(const int& _noiseVolume, const FVector& _posconst )>& _callbackFunc);
	
	//コールバック関数の呼び出し
	void OnNoiseHeard(const int& _noiseVolume, const FVector& _pos);

private:
	//コールバック関数
	//なった瞬間
	std::function<void(const int& _noiseVolume, const FVector& _pos)> m_callBackFunc;	

	
};
