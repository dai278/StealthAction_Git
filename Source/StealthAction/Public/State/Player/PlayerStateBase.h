#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "State/Player/PlayerStatus.h"
#include "Camera/CameraInfo.h"
#include "PlayerStateBase.generated.h"

class APlayerCharacter;
class UPlayerStateMachine;

// Derive a Blueprint from this class (or a native player state) and register it
// in the state-machine Blueprint's State Classes array.
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
	EPlayerStatus GetPlayerStatus() const { return PlayerStatus; }

	UFUNCTION(BlueprintPure, Category = "Player State")
	APlayerCharacter* GetPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "Player State")
	bool RequestPlayerState(EPlayerStatus NewStatus);

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	bool bOverrideCamera = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (EditCondition = "bOverrideCamera"))
	FCameraViewSetting CameraSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", Units = "cm/s"))
	float MoveSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0"))
	float RotationInterpSpeed = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	bool bAllowManualControl = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	bool bAllowMovementInput = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0"))
	int32 FootstepNoiseLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
	float StaminaConsumptionRate = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player State")
	EPlayerStatus PlayerStatus = EPlayerStatus::Idle;

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
	bool CanTransitionToStatus(EPlayerStatus NewStatus) const;
	virtual bool CanTransitionToStatus_Implementation(EPlayerStatus NewStatus) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<APlayerCharacter> Player = nullptr;
};
