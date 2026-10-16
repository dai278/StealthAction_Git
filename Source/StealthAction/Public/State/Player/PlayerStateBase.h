#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "State/Player/PlayerStatus.h"
#include "PlayerStateBase.generated.h"

class APlayerCharacter;

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
	virtual bool CanEnter() const override;
	virtual bool CanExit() const override;
	virtual bool CanTransitionTo(const UStateBase* NewState) const override;

	UFUNCTION(BlueprintPure, Category = "Player State")
	EPlayerStatus GetPlayerStatus() const { return PlayerStatus; }

	UFUNCTION(BlueprintPure, Category = "Player State")
	APlayerCharacter* GetPlayer() const { return Player; }

	UFUNCTION(BlueprintCallable, Category = "Player State")
	bool RequestPlayerState(EPlayerStatus NewStatus);

protected:
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
