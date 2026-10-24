//----------------------------------------------------------
//概要：新しい敵キャラクター
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Damage/IDamage.h"
#include "ModularEnemyCharacter.generated.h"

class AModularEnemyAIController;
class UAnimMontage;

//既存の敵とは別に動かす検証用の敵
UCLASS(Blueprintable)

class STEALTHACTION_API AModularEnemyCharacter : public ACharacter, public IDamageable
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	AModularEnemyCharacter();

	//更新処理
	virtual void Tick(float DeltaTime) override;

	//既存のダメージインターフェース
	virtual void OnDamage(int32 Damage, FVector Knockback, bool bSneakKill) override;

	//Apply Damageからも被ダメージを受ける
	virtual float TakeDamage(float Amount, const FDamageEvent& Event, AController* InstigatorController, AActor* Causer) override;

	//生きているか
	UFUNCTION(BlueprintPure, Category = "Enemy")

	bool IsAlive() const { return Health > 0; }

	//HP取得
	UFUNCTION(BlueprintPure, Category = "Enemy")

	int32 GetHealth() const { return Health; }

	//AIコントローラー取得
	AModularEnemyAIController* GetEnemyController() const;

	//攻撃開始(同じ攻撃は重ねない)
	bool BeginAttack();

	//攻撃中か
	bool IsAttackActive() const { return bAttackActive; }

	//攻撃中断
	void CancelAttack();

	//巡回先を順に取得
	FVector GetNextPatrolLocation();

	//死亡時の共通処理
	void ApplyDeath();

	//探索時の移動速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement", meta = (ClampMin = "0.0"))
	float ExploreSpeed = 180.f;

	//戦闘時の移動速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement", meta = (ClampMin = "0.0"))
	float CombatSpeed = 350.f;

  protected:

	//BeginPlay
	virtual void BeginPlay() override;

	//終了処理
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

	//HP上限
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Damage", meta = (ClampMin = "1"))
	int32 MaximumHealth = 40;

	//検証時に使う巡回地点(レベル上のTargetPointなど)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Enemy|Patrol")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	//攻撃可能な距離
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Attack", meta = (ClampMin = "1.0"))
	float AttackRange = 150.f;

	//攻撃ダメージ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Attack", meta = (ClampMin = "0"))
	int32 AttackDamage = 10;

	//攻撃開始からダメージ判定までの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Attack", meta = (ClampMin = "0.0"))
	float AttackWindup = 0.3f;

	//攻撃全体の時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Attack", meta = (ClampMin = "0.01"))
	float AttackDuration = 0.8f;

	//攻撃終了後の待ち時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Attack", meta = (ClampMin = "0.0"))
	float AttackCooldown = 0.5f;

	//攻撃モンタージュ(未指定でも判定は動く)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Attack")
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	//死亡演出用
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnDeath();

  private:

	//現在のHP
	int32 Health = 40;

	//次の巡回地点の番号
	int32 PatrolIndex = 0;

	//開始位置の保存用
	FVector HomeLocation = FVector::ZeroVector;

	//今回の攻撃対象
	TWeakObjectPtr<AActor> AttackTarget;

	//攻撃開始からの時間
	float AttackElapsed = 0.f;

	//残り待ち時間
	float CooldownRemaining = 0.f;

	//攻撃中か
	bool bAttackActive = false;

	//今回の攻撃でダメージ判定済みか
	bool bDamageApplied = false;

	//死亡処理を実行済みか
	bool bDeathApplied = false;
};
