// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HPGauge/PlayerHPGaugeWidget.h"

#include "Components/ProgressBar.h"

void UPlayerHPGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 初期値（満タン）
	if (HPProgressBar)
	{
		HPProgressBar->SetPercent(1.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HPProgressBar is not bound"));
	}
}

void UPlayerHPGaugeWidget::SetHPPercent(float Percent)
{
	if (!HPProgressBar)
	{
		return;
	}

	// 安全のため 0～1 にクランプ
	const float ClampedPercent = FMath::Clamp(Percent, 0.0f, 1.0f);
	HPProgressBar->SetPercent(ClampedPercent);
}

