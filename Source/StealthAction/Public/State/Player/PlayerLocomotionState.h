#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerLocomotionState.generated.h"

//移動系ステートの共通処理
UCLASS(Abstract, Blueprintable)
class STEALTHACTION_API UPlayerLocomotionState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	//コンストラクタ
	UPlayerLocomotionState();
protected:
	virtual void OnUpdate_Implementation(float DeltaTime) override;
};
