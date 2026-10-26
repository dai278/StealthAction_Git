//担当日髙
//----------------------------------------------------------
//概要：ジャンプステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerLocomotionState.h"
#include "PlayerJumpState.generated.h"

UCLASS(Blueprintable)

class STEALTHACTION_API UPlayerJumpState : public UPlayerLocomotionState
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	UPlayerJumpState();

  protected:

	//ジャンプの初速
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "0.0"))
	float JumpImpulse = 1500.f;

	//ジャンプ中の重力計算に使う値
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "1.0"))
	float GravityCurveBase = 10000.f;

	//落下速度の上限
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "0.0"))
	float MaximumFallSpeed = 1800.f;

	//ジャンプ時の音の大きさ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta = (ClampMin = "0"))
	int32 JumpNoiseLevel = 3;

	//開始条件
	virtual bool CanEnterState_Implementation() const override;

	//開始処理
	virtual void OnEnter_Implementation() override;

	//更新処理
	virtual void OnUpdate_Implementation(float DeltaTime) override;

	//終了処理
	virtual void OnExit_Implementation() override;

  private:

	//入力で開始したジャンプか
	bool bCustomJump = false;

	//変更前の重力倍率
	float SavedGravityScale = 1.f;
};
