#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "Camera/CameraInfo.h"
#include "PlayerStateBase.generated.h"

class APlayerCharacter;
class UPlayerStateMachine;

//プレイヤー用ステートの共通処理
UCLASS(Abstract, BlueprintType, Blueprintable)
class STEALTHACTION_API UPlayerStateBase : public UStateBase
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;
	virtual void Initialize(UStateMachine* StateMachine, AActor* Owner) override;
	virtual void Enter() override;
	virtual void Update(const float& DeltaTime) override;
	virtual void Exit() override;
	virtual void BackgroundUpdate(float DeltaTime) {}
	virtual bool CanEnter() const override;
	virtual bool CanExit() const override;
	virtual bool CanTransitionTo(const UStateBase* NewState) const override;

	UFUNCTION(BlueprintPure, Category = "Player State")
	APlayerCharacter* GetPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "Player State")
	bool RequestPlayerState(TSubclassOf<UPlayerStateBase> NewStateClass);

	UFUNCTION(BlueprintPure, Category = "Player State")
	UPlayerStateMachine* GetPlayerMachine() const;

	UFUNCTION(BlueprintPure, Category = "Player State")
	float GetMoveSpeed() const { return MoveSpeed; }

	UFUNCTION(BlueprintPure, Category = "Player State")
	float GetStaminaConsumptionRate() const { return StaminaConsumptionRate; }

	bool AllowsManualControl() const { return bAllowManualControl; }
	bool UsesCameraOverride() const { return bOverrideCamera; }
	const FCameraViewSetting& GetCameraSettings() const { return CameraSettings; }

	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	void OnMoveInput();
	virtual void OnMoveInput_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	void OnStaminaDepleted();
	virtual void OnStaminaDepleted_Implementation();

protected:
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

	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	void OnEnter();
	virtual void OnEnter_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	void OnUpdate(float DeltaTime);
	virtual void OnUpdate_Implementation(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	void OnExit();
	virtual void OnExit_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	bool CanEnterState() const;
	virtual bool CanEnterState_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	bool CanExitState() const;
	virtual bool CanExitState_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Player State")
	bool CanTransitionToState(TSubclassOf<UPlayerStateBase> NewStateClass) const;
	virtual bool CanTransitionToState_Implementation(TSubclassOf<UPlayerStateBase> NewStateClass) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<APlayerCharacter> Player = nullptr;
};
