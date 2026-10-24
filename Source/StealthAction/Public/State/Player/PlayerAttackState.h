//----------------------------------------------------------
//概要：ステルスキルステート
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerAttackState.generated.h"

UCLASS(Blueprintable)

class STEALTHACTION_API UPlayerAttackState : public UPlayerStateBase
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	UPlayerAttackState();

	//他のステート中も更新する処理
	virtual void BackgroundUpdate(float DeltaTime) override;

	//攻撃終了通知
	void NotifyAttackFinished();

	//攻撃判定が有効か
	bool IsAttackActive() const { return bAttackActive; }
	class AEnemyBase* FindTarget() const;

  protected:

	//攻撃対象を探す距離
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackRange = 150.f;

	//攻撃判定の半径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackRadius = 70.f;

	//ステルスキルのダメージ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0"))
	int32 SneakKillDamage = 10;

	//攻撃時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.01"))
	float AttackDuration = 1.f;

	//次の攻撃までの待ち時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackCooldown = 0.3f;

	//攻撃可能な向きの範囲
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float MinimumFacingDot = 0.4f;

	//攻撃中の重力倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackGravityScale = 0.1f;

	//開始条件
	virtual bool CanEnterState_Implementation() const override;

	//開始処理
	virtual void OnEnter_Implementation() override;

	//更新処理
	virtual void OnUpdate_Implementation(float DeltaTime) override;

	//終了処理
	virtual void OnExit_Implementation() override;

  private:

	//攻撃の残り待ち時間
	float CooldownRemaining = 0.f;

	//変更前の重力倍率
	float SavedGravityScale = 1.f;

	//攻撃判定が有効か
	bool bAttackActive = false;
};
