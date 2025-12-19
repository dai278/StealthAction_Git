// Fill out your copyright notice in the Description page of Project Settings.


//インクルード
#include "MyGameInstance.h"

// コンストラクタ
UMyGameInstance::UMyGameInstance()
{
	// 起動時に初期化
	Reset();
}

// ゲームインスタンスの初期化
void UMyGameInstance::Reset()
{
	// 全フラグを false に初期化
	GameParam.bGoal = false;
	GameParam.bGameOver = false;
	GameParam.bGetKeyItem = false;
}

// =========================
// ゴール関連
// =========================
void UMyGameInstance::SetIsGoal(bool bGoal)
{
	GameParam.bGoal = bGoal;
}

bool UMyGameInstance::GetIsGoal() const
{
	return GameParam.bGoal;
}

// =========================
// ゲームオーバー関連
// =========================
void UMyGameInstance::SetIsGameOver(bool bGameOver)
{
	GameParam.bGameOver = bGameOver;
}

bool UMyGameInstance::GetIsGameOver() const
{
	return GameParam.bGameOver;
}

// =========================
// キーアイテム関連
// =========================
void UMyGameInstance::SetKeyItemFlag(bool bGetKeyItem)
{
	GameParam.bGetKeyItem = bGetKeyItem;

	// フラグ変更を通知（ゴール生成などに使う）
	if (GameParamEventDispatcher.IsBound())
	{
		GameParamEventDispatcher.Broadcast(GameParam.bGetKeyItem);
	}
}

bool UMyGameInstance::GetKeyItemFlag() const
{
	return GameParam.bGetKeyItem;
}
