// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/PlayDataGameInstanceSubsystem.h"

void UPlayDataGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// 初期化処理
}

void UPlayDataGameInstanceSubsystem::Deinitialize()
{
	// 終了処理
	Super::Deinitialize();
}

void UPlayDataGameInstanceSubsystem::BeginPlayHandler(UWorld* World)
{
	// ワールド開始時の処理
}