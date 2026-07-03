// Fill out your copyright notice in the Description page of Project Settings.

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"			//ItemBaseクラス継承
#include <functional>
#include "ItemBase.h"
#include "Camera/CameraInfo.h"
#include "ItemKey.generated.h"

class UWidgetComponent;

class AGoalActor;

//クラスのメンバ関数をキーアイテム取得時のコールバックにするときのマクロ
#define CreateItemKeyNoiseCallback(_func) std::bind(&_func, this)


UCLASS()
class STEALTHACTION_API AItemKey : public AItemBase
{
	//UEのオブジェクトクラスで必ず先頭に書くマクロ
	GENERATED_BODY()

public:
	//コンストラクタ
	AItemKey();


	//---- カメラフォーカス用の情報を返す -----
	FCameraFocusData GetCameraFocusData_Implementation() const
	{

		return FocusProviderComp ? FocusProviderComp->GetCameraFocusData() : FCameraFocusData{};

	}

	//キーアイテム取得時のコールバック関数登録
	void SetOnGetKeyItemCallback(const std::function<void()>& _callbackFunc)
	{
		if (_callbackFunc == nullptr) { return; }
		m_onGetKeyItemCallback = _callbackFunc;
	}

protected:
	// オーバーラップ時
	virtual void HandleOverlap(AActor* OtherActor) override;

	//ゲームスタート時、または生成時に呼ばれる処理
	virtual void BeginPlay() override;


	// ===== ゴール生成用 =====
	UPROPERTY(EditAnywhere, Category = "Goal")
	TSubclassOf<AGoalActor> GoalClass;

	UPROPERTY(EditAnywhere, Category = "Goal")
	FVector GoalSpawnLocation;

	std::function<void()> m_onGetKeyItemCallback;


	//UI
	UPROPERTY(EditAnywhere, Category = "UI")
	UWidgetComponent* KeyUI;


};
