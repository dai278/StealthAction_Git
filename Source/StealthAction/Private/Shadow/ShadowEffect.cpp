// Fill out your copyright notice in the Description page of Project Settings.


#include "Shadow/ShadowEffect.h"
#include "State/Player/PlayerShadowState.h"

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
	APlayerCharacter* player = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (player)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	
}

//--------------------------------
//プレイヤーの状態が変化したときに呼ばれる関数
//--------------------------------
void UShadowEffect::OnPlayerStatusChanged(UPlayerStateBase* _state)
{
	//影の中にいるとき
	if (IsValid(_state) && _state->IsA<UPlayerShadowState>())
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}