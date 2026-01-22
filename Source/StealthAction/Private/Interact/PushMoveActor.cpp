// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/PushMoveActor.h"

//----------------------------------------------------------
// // コンストラクタ
//----------------------------------------------------------
APushMoveActor::APushMoveActor()
	:AInteract()
	, m_moveDistance(100.f)
	, m_moveSpeed(100.f)
	, m_bIsMoving(false)
	, m_targetLocation(FVector::ZeroVector)
	, m_startLocation(FVector::ZeroVector)
	, m_moveProgress(0.f)
	, m_moveDirection(FVector::ZeroVector)
		
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetActorTickEnabled(false);
}


//----------------------------------------------------------
// // BeginPlay
//----------------------------------------------------------
void APushMoveActor::BeginPlay()
{
	Super::BeginPlay();
	
}

//----------------------------------------------------------
// // Tick
//----------------------------------------------------------
void APushMoveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//移動処理
	if (m_bIsMoving)
	{
		m_moveProgress += m_moveSpeed * DeltaTime;
		FVector newLocation = FMath::Lerp(m_startLocation, m_targetLocation, m_moveProgress / m_moveDistance);
		
		FHitResult hit;
		
		SetActorLocation(newLocation,true,&hit);
		
		if(hit.bBlockingHit)
		{
			// 衝突が発生した場合、移動を停止
			m_bIsMoving = false;
			m_moveProgress = 0.f;
			SetActorTickEnabled(false);
			return;
		}
		//移動完了判定
		if (m_moveProgress >= m_moveDistance)
		{
			SetActorLocation(m_targetLocation);
			m_bIsMoving = false;
			m_moveProgress = 0.f;
			SetActorTickEnabled(false);
		}
	}
}


//----------------------------------------------------------
// // インタラクトの実装
//----------------------------------------------------------
void APushMoveActor::Interact(AActor* _interactOwner)
{
 	if (!_interactOwner) { return; }
	if (m_bIsMoving) { return; }
	m_bIsMoving = true;
	SetActorTickEnabled(true);
	

	/*角度floatに変換*/
	FVector Vec = _interactOwner->GetActorLocation()- GetActorLocation();	
	Vec.Z = 0.f;
	
	float deg = FMath::Fmod(FMath::RadiansToDegrees(FMath::Atan2(Vec.Y, Vec.X)) + 360.0f, 360.0f);

	UE_LOG(LogTemp, Warning, TEXT("Vec=(%.3f,%.3f) deg=%.9f IsNaN=%d"),
		Vec.X, Vec.Y, deg, FMath::IsNaN(deg));

	/*移動方向決定90度ごとに*/
	FVector moveDir = FVector::ZeroVector;
	
	//なぜか変数化しないとバグる
	const bool bA = (deg >= 0.f && deg < 45.f);
	const bool bB = (deg >= 315.f && deg < 360.f);

	UE_LOG(LogTemp, Warning, TEXT("deg=%.9f  bA=%d bB=%d"), deg, bA, bB);

	if (bA || bB)
	{
		UE_LOG(LogTemp, Warning, TEXT("BRANCH: FIRST IF"));
		moveDir = FVector(-1.f, 0.f, 0.f);
	}
	else if (deg >= 45.f && deg < 135.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BRANCH: 90-180"));
		moveDir = FVector(0.f, -1.f, 0.f);
	}
	else if (deg >= 135.f && deg < 225.f) // ←ここは後述の通り修正して
	{
		UE_LOG(LogTemp, Warning, TEXT("BRANCH: 180-270"));
		moveDir = FVector(1.f, 0.f, 0.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BRANCH: ELSE"));
		moveDir = FVector(0.f, 1.f, 0.f); // デバッグ用：見分けるため
	}
	

	
	m_moveDirection = moveDir;
	m_startLocation = GetActorLocation();
	m_targetLocation = m_startLocation + m_moveDirection * m_moveDistance;
}