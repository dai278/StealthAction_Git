#pragma once

#include "CoreMinimal.h"
#include "State/Player/PlayerStateBase.h"
#include "Engine/EngineTypes.h"
#include "PlayerShadowState.generated.h"

UCLASS(Blueprintable)
class STEALTHACTION_API UPlayerShadowState : public UPlayerStateBase
{
	GENERATED_BODY()
public:
	UPlayerShadowState();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	FName ShadowCollisionProfile = TEXT("PlayerInShadow");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	TObjectPtr<class USkeletalMesh> ShadowMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shadow")
	TSubclassOf<AActor> ShadowEffectClass;
	virtual bool CanEnterState_Implementation() const override;
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnExit_Implementation() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<class USkeletalMesh> OriginalMesh = nullptr;
	FName OriginalCollisionProfile = NAME_None;
	ECollisionEnabled::Type OriginalMeshCollision = ECollisionEnabled::QueryOnly;
	bool bOriginalOverlapEvents = false;
	bool bShadowApplied = false;
};
