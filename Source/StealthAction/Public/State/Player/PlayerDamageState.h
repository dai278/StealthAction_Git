//----------------------------------------------------------
//概要：被ダメージステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerDamageState.generated.h"

UCLASS(Blueprintable)

class STEALTHACTION_API UPlayerDamageState : public UPlayerStateBase
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	UPlayerDamageState();

	//他のステート中も更新する処理
	virtual void BackgroundUpdate(float DeltaTime) override;

	//被ダメージ処理
	void ReceiveDamage(int32 Amount, const FVector& Knockback);

	//無敵中か
	bool IsInvincible() const { return InvincibilityRemaining > 0.f; }

	//無敵解除
	void ClearProtection();

  protected:

	//被ダメージから復帰するまでの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float RecoveryDuration = 1.f;

	//無敵時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float InvincibilityDuration = 5.f;

	//ノックバックの倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float KnockbackMultiplier = 1.f;

	//開始処理
	virtual void OnEnter_Implementation() override;

	//更新処理
	virtual void OnUpdate_Implementation(float DeltaTime) override;

  private:

	//残り無敵時間
	float InvincibilityRemaining = 0.f;

	//被ダメージ時に反映するノックバック
	FVector PendingKnockback = FVector::ZeroVector;
};
