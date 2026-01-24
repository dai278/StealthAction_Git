// Fill out your copyright notice in the Description page of Project Settings.


#include "Shadow/ShadowEffect.h"

#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

//--------------------------------
//コンストラクタ
//--------------------------------
UShadowEffect::UShadowEffect(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::Visible);
}


//--------------------------------
//BeginPlay
//--------------------------------
void UShadowEffect::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//プレイヤーキャラクターの取得
	APlayerCharacter* player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (player)
	{
		player->OnPlayerConditionMet.AddDynamic(this, &UShadowEffect::OnPlayerStatusChanged);
		SetVisibility(ESlateVisibility::Hidden);

	}
	
}

//--------------------------------
//プレイヤーの状態が変化したときに呼ばれる関数
//--------------------------------
void UShadowEffect::OnPlayerStatusChanged(EPlayerStatus _state)
{
	//影の中にいるとき
	if (_state == EPlayerStatus::InShadow)
	{
		SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Display, TEXT("KageEfeect On"));
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Display, TEXT("KageEfeect OFF"));

	}
}