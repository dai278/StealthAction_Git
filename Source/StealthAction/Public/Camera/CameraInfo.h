#pragma once

#include "CoreMinimal.h"
#include "CameraInfo.generated.h"

// ƒJƒƒ‰‹“_İ’è\‘¢‘Ì
USTRUCT(BlueprintType)
struct FCameraViewSetting
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator rotator; // ‰ñ“]

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float fieldOfView = 90.0f; // ‹–ìŠp

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float springArmLength = 300.0f; // ’·‚³

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FVector ofSet;
};
