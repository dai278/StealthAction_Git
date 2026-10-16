#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "PlayerStates.generated.h"

// Shared locomotion selection; movement itself still uses Enhanced Input.
UCLASS(Abstract, Blueprintable)
class STEALTHACTION_API UPlayerLocomotionState : public UPlayerStateBase
{
	GENERATED_BODY()
protected:
	virtual void OnUpdate_Implementation(float DeltaTime) override;
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerIdleState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerIdleState();
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerWalkState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerWalkState();
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerDashState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerDashState();
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerJumpState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerJumpState();
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerCrouchState : public UPlayerLocomotionState
{
	GENERATED_BODY()
public:
	UPlayerCrouchState();
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerAttackState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerAttackState();
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerDamageState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerDamageState();
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerDeadState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerDeadState();
protected:
	virtual void OnEnter_Implementation() override;
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerShadowState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerShadowState();
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerInteractState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerInteractState();
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerInteractAnimationState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerInteractAnimationState();
protected:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;
};
