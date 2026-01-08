// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHPGaugeWidget.generated.h"

class UProgressBar;
class APlayerCharacter;

UCLASS()
class STEALTHACTION_API UPlayerHPGaugeWidget : public UUserWidget
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
	// HPバー（UMGのProgressBarとバインド）
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPProgressBar;

	// プレイヤーキャラクター参照
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	// 最大HP（初期化時に取得）
	int32 MaxHP;

	// HP更新処理
	void UpdateHP();
};
