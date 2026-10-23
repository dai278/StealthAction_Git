#pragma once

#include "CoreMinimal.h"
#include "PlayerStatus.generated.h"

//AnimBP用の状態(ステートの切り替えには使わない)
UENUM(BlueprintType)
enum class EPlayerStatus :uint8
{
	Idle         UMETA(DisplayName = "Idle"),
	Walk         UMETA(DisplayName = "Walk"),
	Dash         UMETA(DisplayName = "Dash"),
	Jump         UMETA(DisplayName = "Jump"),
	Crouch       UMETA(DisplayName = "Crouch"),
	Attack       UMETA(DisplayName = "Attack"),
	Damage       UMETA(DisplayName = "Damage"),
	Dead         UMETA(DisplayName = "Dead"),
	InShadow     UMETA(DisplayName = "InShadow"),
	Interact     UMETA(DisplayName = "Interact"),
	InteractAnimation UMETA(DisplayName = "InteractAnimation"),
};
