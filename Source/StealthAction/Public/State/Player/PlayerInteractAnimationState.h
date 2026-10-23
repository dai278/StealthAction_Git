#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerInteractAnimationState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerInteractAnimationState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	//コンストラクタ
	UPlayerInteractAnimationState();
	//対象を保存してインタラクト開始
	bool BeginInteraction(class AInteract* Target);
	//インタラクト実行
	void CompleteInteraction();

protected:
	//インタラクト実行までの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = "0.0"))
	float InteractionDuration = 2.5f;
	//時間ではなくアニメーション通知を待つか
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bWaitForAnimationNotify = false;
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	//インタラクト対象の保存用
	TWeakObjectPtr<class AInteract> InteractionTarget;
	//インタラクトを実行済みか
	bool bInteractionExecuted = false;
};
