// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShadowTimeWidget.generated.h"

class UProgressBar;
class APlayerCharacter;

UCLASS()
class STEALTHACTION_API UShadowTimeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Widget生成時
	virtual void NativeConstruct() override;

	// 毎フレーム更新
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

private:
	// プログレスバー
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShadowTimeBar;

	// プレイヤー参照
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	// 更新処理
	void UpdateShadowTime();
};
