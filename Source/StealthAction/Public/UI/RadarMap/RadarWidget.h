#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadarWidget.generated.h"

class AEnemy_1;
class APlayerCharacter;

UCLASS()
class STEALTHACTION_API URadarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// コンストラクタ
	URadarWidget(const FObjectInitializer& ObjectInitializer);

	// 所有プレイヤー設定（※今回は未使用でもOK）
	UFUNCTION(BlueprintCallable)
	void SetOwnerPlayer(APlayerCharacter* Player);

	// ===== レーダー設定 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar")
	float RadarRadius = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar")
	float RadarRange = 1500.f;

protected:
	virtual void NativeConstruct() override;

	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const override;

protected:
	UPROPERTY()
	APlayerCharacter* OwnerPlayer = nullptr;
};
