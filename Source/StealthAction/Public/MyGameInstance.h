// Fill out your copyright notice in the Description page of Project Settings.

// インクルードガード
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/*
 * ゲーム全体で共有したいフラグをまとめた構造体
 * レベルが変わっても保持される
 */
struct FGameParam
{
	bool bGoal;         // ゴール到達フラグ
	bool bGameOver;     // ゲームオーバーフラグ
	bool bGetKeyItem;   // キーアイテム取得フラグ
};

/*
 * パラメータ変更通知用のイベント
 * bool を1つ渡す
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FGameParamEventDispatcher, bool);

UCLASS()
class STEALTHACTION_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UMyGameInstance();

	// =========================
	// 初期化
	// =========================
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void Reset();

	// =========================
	// ゴール関連
	// =========================
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void SetIsGoal(bool bGoal);

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	bool GetIsGoal() const;

	// =========================
	// ゲームオーバー関連
	// =========================
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void SetIsGameOver(bool bGameOver);

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	bool GetIsGameOver() const;

	// =========================
	// キーアイテム関連
	// =========================
	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	void SetKeyItemFlag(bool bGetKeyItem);

	UFUNCTION(BlueprintCallable, Category = "GameInstance")
	bool GetKeyItemFlag() const;

public:
	/*
	 * フラグが変更された時に通知されるイベント
	 * 例：キー取得 → ゴール生成
	 */
	FGameParamEventDispatcher GameParamEventDispatcher;

private:
	// ゲーム共通パラメータ
	FGameParam GameParam;
};
