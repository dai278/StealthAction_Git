// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HPGauge/PlayerHPGaugeWidget.h"

#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

void UPlayerHPGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// PlayerCharacter取得
	PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
	);

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter not found"));
		return;
	}

	// 最大HPを取得（初期値を最大HPとして扱う）
	MaxHP = PlayerCharacter->GetPlayerInfo().hp;

	// 初期表示
	UpdateHP();
}

void UPlayerHPGaugeWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateHP();
}

void UPlayerHPGaugeWidget::UpdateHP()
{
	if (!PlayerCharacter || !HPProgressBar || MaxHP <= 0)
	{
		return;
	}

	const int32 CurrentHP = PlayerCharacter->GetPlayerInfo().hp;
	const float HPPercent = FMath::Clamp(
		static_cast<float>(CurrentHP) / static_cast<float>(MaxHP),
		0.0f,
		1.0f
	);

	HPProgressBar->SetPercent(HPPercent);
}
