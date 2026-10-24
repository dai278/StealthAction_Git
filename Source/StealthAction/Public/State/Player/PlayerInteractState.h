#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerInteractState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerInteractState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	//コンストラクタ
	UPlayerInteractState();
	//BPで開始条件を上書きしても対象なしでは開始しない
	virtual bool CanEnter() const override;

protected:
	//インタラクト位置に着いたと判定する距離
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "0.0"))
	float AcceptanceRadius = 50.f;
	//インタラクト位置に向かう移動の強さ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ApproachInputScale = 0.5f;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	//インタラクト対象の保存用
	TWeakObjectPtr<class AInteract> InteractionTarget;
	//インタラクト位置の保存用
	FVector InteractionPosition = FVector::ZeroVector;
};
