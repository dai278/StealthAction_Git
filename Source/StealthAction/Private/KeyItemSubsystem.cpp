// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyItemSubsystem.h"

//ベギンプレイ
void UKeyItemSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	keyItem = nullptr;
}

//キーアイテムを登録する関数
void UKeyItemSubsystem::RegisterKeyItem(AItemKey* _keyItem)
{
	keyItem = _keyItem;
}



//キーアイテムを取得する関数
AItemKey* UKeyItemSubsystem::GetKeyItem()
{
	if (keyItem)
	{
		return keyItem;
	}
	return nullptr;
}
