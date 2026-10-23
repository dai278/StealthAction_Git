#pragma once

#include "CoreMinimal.h"
#include "State/StateMachineComponent.h"
#include "PlayerStateMachine.generated.h"

class UPlayerStateBase;

UCLASS(BlueprintType, Blueprintable)
class STEALTHACTION_API UPlayerStateMachine : public UStateMachine
{
	GENERATED_BODY()

public:
	//コンストラクタ
	UPlayerStateMachine();
	//初期化
	virtual void Initialize(AActor* Owner) override;
	//終了処理
	virtual void Stop() override;
	//更新処理
	virtual void Update(float DeltaTime) override;

	//登録済みステート取得
	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	UPlayerStateBase* GetPlayerState(TSubclassOf<UPlayerStateBase> StateClass) const;

	//操作可能か
	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool CanControl() const;

	//スタミナ消費量取得
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetStaminaUsed() const { return StaminaUsed; }

	//スタミナ上限取得
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaximumStamina() const { return FMath::Max(0.f, MaximumStamina); }

	//スタミナ切れか
	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool IsStaminaDepleted() const { return MaximumStamina <= 0.f || StaminaUsed >= MaximumStamina; }

	//無敵中か
	bool IsInvincible() const;
	//攻撃中か
	bool IsAttacking() const;
	//ジャンプ入力があったか
	bool IsJumpRequested() const { return bJumpRequested; }
	//移動入力
	void MoveInput();
	//ダッシュ入力の保存
	void SetDashRequested(bool bRequested);
	//しゃがみ切り替え
	void ToggleCrouch();
	//ジャンプ開始
	void TryJump();
	//攻撃開始
	void TryAttack();
	//影状態切り替え
	void ToggleShadow();
	//インタラクト開始
	void TryInteract();
	//着地通知
	void NotifyLanded();
	//攻撃終了通知
	void NotifyAttackFinished();
	//インタラクト終了通知
	void NotifyInteractionFinished();
	//被ダメージ処理
	void ReceiveDamage(int32 Amount, const FVector& Knockback);
	//入力に合わせて移動ステートを変更
	void RefreshLocomotionState();
	//移動ステートに戻す
	bool ReturnToLocomotion(bool bAfterLanding = false);
	//ダッシュ解除
	void CancelDash() { bDashRequested = false; }
	//ダッシュとしゃがみの要求を解除
	void CancelLocomotionRequests() { bDashRequested = false; bCrouchRequested = false; }

	//ステート変更処理
	UFUNCTION(BlueprintCallable, Category = "Player State Machine")
	bool ChangePlayerState(TSubclassOf<UPlayerStateBase> NewStateClass);

	//移動系ステートか
	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool IsLocomotionState() const;

	//登録が完了しているか
	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool IsConfigured() const { return bConfigured; }

protected:
	//開始ステート
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State Machine")
	TSubclassOf<UPlayerStateBase> InitialStateClass;

	//スタミナ上限
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
	float MaximumStamina = 5.f;

	//スタミナ回復速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
	float StaminaRecoveryRate = 1.5f;

private:
	//移動ステート選択
	TSubclassOf<UPlayerStateBase> SelectLocomotionState(bool bAfterLanding = false) const;
	//スタミナ消費量
	float StaminaUsed = 0.f;
	//ダッシュの要求があるか
	bool bDashRequested = false;
	//しゃがみの要求があるか
	bool bCrouchRequested = false;
	//ジャンプの要求があるか
	bool bJumpRequested = false;

	//ステートの登録が完了したか
	bool bConfigured = false;
	//ステート変更処理中か
	bool bTransitionInProgress = false;
};
