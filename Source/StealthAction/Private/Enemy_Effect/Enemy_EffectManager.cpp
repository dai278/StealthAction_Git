#include "Enemy_Effect/Enemy_EffectManager.h"
#include "Enemy_Effect/Enemy_Effect_1.h"
#include "Enemy_Effect/Enemy_Effect_2.h"
#include "Enemy_Effect/Enemy_Effect_3.h"
#include "Enemy_Effect/Enemy_Effect_4.h"
#include "Engine/World.h"

AEnemy_EffectManager::AEnemy_EffectManager()
	: m_effect1Stock(10)
	, m_effect2Stock(10)
	, m_effect3Stock(10)
	, m_effect4Stock(10)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemy_EffectManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		//UE_LOG(LogTemp, Error, TEXT("AEnemy_EffectManager::BeginPlay World is null"));
		return;
	}

	if (!Effect1Class) //UE_LOG(LogTemp, Warning, TEXT("Effect1Class is null (skip prewarm)"));
	if (!Effect2Class) //UE_LOG(LogTemp, Warning, TEXT("Effect2Class is null (skip prewarm)"));
	if (!Effect3Class) //UE_LOG(LogTemp, Warning, TEXT("Effect3Class is null (skip prewarm)"));
	if (!Effect4Class) //UE_LOG(LogTemp, Warning, TEXT("Effect4Class is null (skip prewarm)"));

	PrewarmPoolTyped<AEnemy_Effect_1>(Effect1Class, m_effect1Stock, Effect1Pool);
	PrewarmPoolTyped<AEnemy_Effect_2>(Effect2Class, m_effect2Stock, Effect2Pool);
	PrewarmPoolTyped<AEnemy_Effect_3>(Effect3Class, m_effect3Stock, Effect3Pool);
	PrewarmPoolTyped<AEnemy_Effect_4>(Effect4Class, m_effect4Stock, Effect4Pool);
}

template<typename T>
void AEnemy_EffectManager::PrewarmPoolTyped(TSubclassOf<T> Class, int32 Stock, TArray<T*>& Pool)
{
	UWorld* World = GetWorld();
	if (!World || !Class || Stock <= 0)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Pool.Reserve(Pool.Num() + Stock);

	for (int32 i = 0; i < Stock; ++i)
	{
		T* Effect = World->SpawnActor<T>(Class, FVector::ZeroVector, FRotator::ZeroRotator, Params);
		if (!IsValid(Effect))
		{
			continue;
		}

		// 未使用状態にする（Effect側で別の初期化があるなら呼んでも良い）
		Effect->SetActorHiddenInGame(true);
		Pool.Add(Effect);
	}
}

AEnemy_Effect_1* AEnemy_EffectManager::GetEffect1()
{
	for (AEnemy_Effect_1* Effect : Effect1Pool)
	{
		if (IsValid(Effect) && Effect->IsHidden())
		{
			// 予約（同フレーム二重取得対策）
			Effect->SetActorHiddenInGame(false);
			return Effect;
		}
	}
	return nullptr;
}

AEnemy_Effect_2* AEnemy_EffectManager::GetEffect2()
{
	for (AEnemy_Effect_2* Effect : Effect2Pool)
	{
		if (IsValid(Effect) && Effect->IsHidden())
		{
			Effect->SetActorHiddenInGame(false);
			return Effect;
		}
	}
	return nullptr;
}

AEnemy_Effect_3* AEnemy_EffectManager::GetEffect3()
{
	for (AEnemy_Effect_3* Effect : Effect3Pool)
	{
		if (IsValid(Effect) && Effect->IsHidden())
		{
			Effect->SetActorHiddenInGame(false);
			return Effect;
		}
	}
	return nullptr;
}

AEnemy_Effect_4* AEnemy_EffectManager::GetEffect4()
{
	for (AEnemy_Effect_4* Effect : Effect4Pool)
	{
		if (IsValid(Effect) && Effect->IsHidden())
		{
			Effect->SetActorHiddenInGame(false);
			return Effect;
		}
	}
	return nullptr;
}