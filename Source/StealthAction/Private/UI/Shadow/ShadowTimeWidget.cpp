#include "UI/Shadow/ShadowTimeWidget.h"

#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

void UShadowTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// PlayerCharacterŽæ“¾
	PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
	);

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("ShadowTimeWidget: PlayerCharacter not found"));
	}
}

void UShadowTimeWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateShadowTime();
}

void UShadowTimeWidget::UpdateShadowTime()
{
	if (!PlayerCharacter || !ShadowTimeBar)
	{
		return;
	}

	//Œ»Ý‚Ì‰e‘ØÝŽžŠÔ‚ðŽæ“¾
	const float Current = PlayerCharacter->GetShadowTimer();

	//Å‘å‰e‘ØÝŽžŠÔ‚ðŽæ“¾
	const float Max = PlayerCharacter->GetMaxShadowTime();

	if (Max <= 0.f)
	{
		return;
	}

	const float Rate = FMath::Clamp(Current / Max, 0.f, 1.f);
	ShadowTimeBar->SetPercent(Rate);
}
