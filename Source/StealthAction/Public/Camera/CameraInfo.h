#pragma once

#include "CoreMinimal.h"
#include "CameraInfo.generated.h"

// カメラ視点設定構造体
USTRUCT(BlueprintType)
struct FCameraViewSetting
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator rotator; // 回転

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float fieldOfView = 90.0f; // 視野角

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float springArmLength = 300.0f; // 長さ

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FVector ofSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float InterpSpeed = 5.f; // カメラ切り替えのブレンド時間
};
