// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHPGaugeWidget.generated.h"

class UProgressBar;

UCLASS()
class STEALTHACTION_API UPlayerHPGaugeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	//Widget¶¬‚ÉŒÄ‚Î‚ê‚é
	virtual void NativeConstruct() override;

public:
	//HPŠ„‡‚ğİ’è
	UFUNCTION(BlueprintCallable, Category = "HP")
	void SetHPPercent(float Percent);

protected:
	//UMG‘¤‚ÌProgressBar‚Æ•R‚Ã‚¯‚é
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPProgressBar;

};
