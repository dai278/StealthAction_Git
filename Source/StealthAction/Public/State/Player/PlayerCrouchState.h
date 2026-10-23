#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerCrouchState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerCrouchState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	//コンストラクタ
	UPlayerCrouchState();

protected:
	//しゃがみ中のカプセル高さの倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float CapsuleHeightRatio = 0.8f;

	virtual void OnEnter_Implementation() override;
	virtual void OnExit_Implementation() override;

};
