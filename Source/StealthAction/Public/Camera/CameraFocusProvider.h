#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraFocusProvider.generated.h"


UENUM(BlueprintType)
enum class ERewindTriggerType : uint8
{
    None        UMETA(DisplayName = "None"),
    Manual      UMETA(DisplayName = "Manual"),
    TimedAuto   UMETA(DisplayName = "Timed Auto"),
};

USTRUCT(BlueprintType)
struct FCameraFocusData
{
    GENERATED_BODY()

 
    // ギミックごとに置くカメラ
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<class ACameraActor> FocusCamera = nullptr;


    // 補間時間
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BlendTime = 10.f;

	// 戻るときの補間時間
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BlendOutTime = 0.3f;

	// 位置を強制的に合わせるか
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bForceLocation = false;

    // 強制的に回転させるか
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bForceRotation = false;

    //　一時停止するか
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    bool bPause=false;

    //ポーズ時間
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PauseTime = 5.f;

	//巻き戻しトリガーのタイプ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERewindTriggerType RewindTriggerType = ERewindTriggerType::TimedAuto;
};

// 動的デリゲートの宣言例
DECLARE_DYNAMIC_DELEGATE_OneParam(FObtainedDynamicDelegate, FCameraFocusData, _cameraFocusData);


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