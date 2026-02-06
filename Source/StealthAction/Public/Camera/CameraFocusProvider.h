#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraFocusProvider.generated.h"

USTRUCT(BlueprintType)
struct FCameraFocusData
{
    GENERATED_BODY()

    // フォーカスする位置（Actorの位置＋オフセット用）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector FocusLocationOffset = FVector::ZeroVector;

    // フォーカスする回転（未指定ならカメラ側で計算）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator FocusRotation = FRotator::ZeroRotator;

    // 補間時間
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BlendTime = 0.5f;

    // 強制的に回転させるか
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bForceRotation = false;

    //　一時停止するか
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    bool bPouse;
};


UINTERFACE(BlueprintType)
class UCameraFocusProvider : public UInterface
{
    GENERATED_BODY()
};

class ICameraFocusProvider
{
    GENERATED_BODY()

public:

    // カメラフォーカス用の情報を返す
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Camera")
    FCameraFocusData GetCameraFocusData() const;
};