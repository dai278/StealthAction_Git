// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyItemSubsystem.h"
#include "ItemKey.h"

//ベギンプレイ
void UKeyItemSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	keyItem = nullptr;
}

//キーアイテムを登録する関数
void UKeyItemSubsystem::RegisterKeyItem(AItemKey* _keyItem)
{
	keyItem = IsValid(_keyItem) ? _keyItem : nullptr;
}



//キーアイテムを取得する関数
AItemKey* UKeyItemSubsystem::GetKeyItem()
{
	if (IsValid(keyItem))
	{
		return keyItem;
	}
	return nullptr;
}
