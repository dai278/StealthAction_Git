#pragma once

#include "CoreMinimal.h"
#include "State/StateMachineComponent.h"
#include "State/Player/PlayerStatus.h"
#include "PlayerStateMachine.generated.h"

class UPlayerStateBase;

UCLASS(BlueprintType, Blueprintable)
class STEALTHACTION_API UPlayerStateMachine : public UStateMachine
{
	GENERATED_BODY()

public:
	virtual void Initialize(AActor* Owner) override;
	virtual void Stop() override;

	UFUNCTION(BlueprintCallable, Category = "Player State Machine")
	bool ChangePlayerState(EPlayerStatus NewStatus);

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	EPlayerStatus GetPlayerStatus() const;

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool IsLocomotionState() const;

	UFUNCTION(BlueprintPure, Category = "Player State Machine")
	bool IsConfigured() const { return bConfigured; }

private:
	UPROPERTY(Transient)
	TMap<EPlayerStatus, TObjectPtr<UPlayerStateBase>> PlayerStates;

	EPlayerStatus InitialStatus = EPlayerStatus::Idle;
	bool bConfigured = false;
	bool bTransitionInProgress = false;
};
