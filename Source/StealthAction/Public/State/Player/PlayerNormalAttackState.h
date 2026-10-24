#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerNormalAttackState.generated.h"

class UAnimInstance;
class UAnimMontage;

//敵に見つかっているときなどに使う通常攻撃
UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerNormalAttackState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerNormalAttackState();
	virtual bool CanEnter() const override;
	virtual void BackgroundUpdate(float DeltaTime) override;
	bool IsAttackActive() const { return bAttackActive; }

protected:
	//通常攻撃のモンタージュ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Animation")
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;
	//再生速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Animation", meta = (ClampMin = "0.01"))
	float MontagePlayRate = 1.f;
	//中断時のブレンド時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Animation", meta = (ClampMin = "0.0"))
	float MontageBlendOutTime = 0.1f;
	//モンタージュ未設定時の攻撃時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.01"))
	float AttackDuration = 0.6f;
	//次の攻撃までの待ち時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float AttackCooldown = 0.3f;
	//通常攻撃のダメージ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0"))
	int32 AttackDamage = 10;
	//攻撃開始から判定を出すまでの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Hit", meta = (ClampMin = "0.0", Units = "s"))
	float DamageStartTime = 0.15f;
	//攻撃判定を出している時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Hit", meta = (ClampMin = "0.0", Units = "s"))
	float DamageWindowDuration = 0.15f;
	//判定の開始位置(プレイヤー基準)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Hit")
	FVector AttackOffset = FVector(40.f, 0.f, 0.f);
	//前方へ判定を伸ばす距離
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Hit", meta = (ClampMin = "0.0", Units = "cm"))
	float AttackDistance = 100.f;
	//攻撃判定の半径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Hit", meta = (ClampMin = "1.0", Units = "cm"))
	float AttackRadius = 50.f;
	//ノックバックの強さ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Hit", meta = (ClampMin = "0.0"))
	float KnockbackForce = 500.f;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	void ApplyAttackHits();
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	TWeakObjectPtr<UAnimInstance> PlayingAnimInstance;
	//複数のコリジョンに当たっても同じ敵には1回だけ
	TSet<TWeakObjectPtr<AActor>> HitActors;
	float CooldownRemaining = 0.f;
	float ActiveDuration = 0.f;
	float PreviousAttackTime = 0.f;
	bool bAttackActive = false;
	bool bAttackCancelled = false;
	bool bMontageEnded = false;
};
