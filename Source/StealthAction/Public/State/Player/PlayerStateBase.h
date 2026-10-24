//----------------------------------------------------------
//概要：プレイヤーステートの共通処理
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "Camera/CameraInfo.h"
#include "State/Player/PlayerStatus.h"
#include "PlayerStateBase.generated.h"

class APlayerCharacter;
class UPlayerStateMachine;

//プレイヤー用ステートの共通処理
UCLASS(Abstract, BlueprintType, Blueprintable)

class STEALTHACTION_API UPlayerStateBase : public UStateBase
{
	GENERATED_BODY()

  public:

	//AnimBP用の状態取得
	UFUNCTION(BlueprintPure, Category = "Player State|Animation")

	EPlayerStatus GetPlayerStatus() const { return PlayerStatus; }

	//ワールド取得
	virtual UWorld* GetWorld() const override;

	//初期化
	virtual void Initialize(UStateMachine* StateMachine, AActor* Owner) override;

	//開始処理
	virtual void Enter() override;

	//終了処理
	virtual void Exit() override;

	//他のステート中も更新する処理
	virtual void BackgroundUpdate(float DeltaTime) {}

	//開始可能か
	virtual bool CanEnter() const override;

	//遷移可能か
	virtual bool CanTransitionTo(const UStateBase* NewState) const override;

	//プレイヤー取得
	UFUNCTION(BlueprintPure, Category = "Player State")
	APlayerCharacter* GetPlayer() const;

	//ステート変更要求
	UFUNCTION(BlueprintCallable, Category = "Player State")
	bool RequestPlayerState(TSubclassOf<UPlayerStateBase> NewStateClass);

	//ステートマシン取得
	UFUNCTION(BlueprintPure, Category = "Player State")
	UPlayerStateMachine* GetPlayerMachine() const;

	//移動速度取得
	UFUNCTION(BlueprintPure, Category = "Player State")

	float GetMoveSpeed() const { return MoveSpeed; }

	//スタミナ消費速度取得
	UFUNCTION(BlueprintPure, Category = "Player State")

	float GetStaminaConsumptionRate() const { return StaminaConsumptionRate; }

	//操作可能か
	bool AllowsManualControl() const { return bAllowManualControl; }

	//専用のカメラ設定を使うか
	bool UsesCameraOverride() const { return bOverrideCamera; }

	//カメラ設定取得
	const FCameraViewSetting& GetCameraSettings() const { return CameraSettings; }

	//移動入力処理
	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	void OnMoveInput();

	//移動入力処理
	virtual void OnMoveInput_Implementation();

	//スタミナ切れ処理
	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	void OnStaminaDepleted();

	//スタミナ切れ処理
	virtual void OnStaminaDepleted_Implementation();

  protected:

	//このステートに対応するAnimBP用の状態
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	EPlayerStatus PlayerStatus = EPlayerStatus::Idle;

	//カメラ設定を切り替えるか
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	bool bOverrideCamera = false;

	//カメラ設定
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (EditCondition = "bOverrideCamera"))
	FCameraViewSetting CameraSettings;

	//移動速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", Units = "cm/s"))
	float MoveSpeed = 300.f;

	//向きの補間速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float RotationInterpSpeed = 10.f;

	//操作可能か
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	bool bAllowManualControl = true;

	//移動入力を受け付けるか
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	bool bAllowMovementInput = false;

	//足音の大きさ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0"))
	int32 FootstepNoiseLevel = 0;

	//スタミナ消費速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
	float StaminaConsumptionRate = 0.f;


  private:

	//プレイヤーの保存用
	UPROPERTY(Transient)
	TObjectPtr<APlayerCharacter> Player = nullptr;
};
