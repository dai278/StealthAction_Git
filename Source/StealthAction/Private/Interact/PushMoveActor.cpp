//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/PushMoveActor.h"
#include "Components/BoxComponent.h"

//----------------------------------------------------------
// // ƒRƒ“ƒXƒgƒ‰ƒNƒ^
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

	InteractPos.SetNum(4);

}


//----------------------------------------------------------
// // BeginPlay
//----------------------------------------------------------
void APushMoveActor::BeginPlay()
{
	Super::BeginPlay();
	
	//‰ŠúˆÊ’u•Û‘¶
	m_initPos = GetActorLocation();
}

//----------------------------------------------------------
// // Tick
//----------------------------------------------------------
void APushMoveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_bIsMoving)
	{
		return;
	}

	//ƒtƒŒ[ƒ€—‚¿‚µ‚Ä‚à–Ú•WˆÊ’u‚ğ’Ê‚è‰z‚³‚È‚¢‚æ‚¤‚É‚·‚é
	m_moveProgress = FMath::Min(m_moveProgress + m_moveSpeed * FMath::Max(0.f, DeltaTime), m_moveDistance);
	const FVector newLocation = FMath::Lerp(m_startLocation, m_targetLocation, m_moveProgress / m_moveDistance);
	FHitResult hit;
	SetActorLocation(newLocation, true, &hit);

	//•Ç‚É“–‚½‚Á‚½‚ÆˆÚ“®‚ªI‚í‚Á‚½‚Í~‚ß‚é
	if (hit.bBlockingHit || m_moveProgress >= m_moveDistance)
	{
		m_bIsMoving = false;
		m_moveProgress = 0.f;
		SetActorTickEnabled(false);
	}
}

//----------------------------------------------------------
// ƒCƒ“ƒ^ƒ‰ƒNƒg‚ÌÀ‘•
//----------------------------------------------------------
void APushMoveActor::Interact(AActor* _interactOwner)
{
	if (!IsValid(_interactOwner) || !IsValid(m_bodyCollision) || m_bIsMoving)
	{
		return;
	}
	if (m_moveDistance <= 0.f || m_moveSpeed <= 0.f)
	{
		return;
	}

	//ƒ[ƒ‹ƒh‚ÌŠp“x‚Å‚Í‚È‚­A–{‘ÌƒRƒŠƒWƒ‡ƒ“‚©‚çŒ©‚½ˆÊ’u‚Å‰Ÿ‚·–Ê‚ğŒˆ‚ß‚é
	const FTransform bodyTransform = m_bodyCollision->GetComponentTransform();
	FVector localPosition = bodyTransform.InverseTransformPosition(_interactOwner->GetActorLocation());
	localPosition.Z = 0.f;
	if (localPosition.IsNearlyZero())
	{
		return;
	}

	//” ‚ÌŒü‚«‚É‰ˆ‚Á‚½“ñ•ûŒü‚ğ—pˆÓ‚·‚éB‚‚³•ûŒü‚É‚Í“®‚©‚³‚È‚¢
	const FVector extent = m_bodyCollision->GetUnscaledBoxExtent();
	const FVector localX(localPosition.X > 0.f ? -1.f : 1.f, 0.f, 0.f);
	const FVector localY(0.f, localPosition.Y > 0.f ? -1.f : 1.f, 0.f);
	FVector pushX = bodyTransform.TransformVector(localX);
	FVector pushY = bodyTransform.TransformVector(localY);
	pushX.Z = 0.f;
	pushY.Z = 0.f;
	pushX = pushX.GetSafeNormal();
	pushY = pushY.GetSafeNormal();

	const bool outsideX = FMath::Abs(localPosition.X) >= extent.X;
	const bool outsideY = FMath::Abs(localPosition.Y) >= extent.Y;
	if (outsideX && outsideY)
	{
		//Šp‚Å‚Í’†S‚©‚ç‚ÌŠp“x‚ÅŒˆ‚ß‚¸AƒvƒŒƒCƒ„[‚ªŒü‚¢‚Ä‚¢‚é•û‚Ö‰Ÿ‚·
		FVector facing = _interactOwner->GetActorForwardVector();
		facing.Z = 0.f;
		facing = facing.GetSafeNormal();
		const float facingX = FVector::DotProduct(facing, pushX);
		const float facingY = FVector::DotProduct(facing, pushY);
		if (FMath::Max(facingX, facingY) <= 0.f)
		{
			return;
		}
		m_moveDirection = facingX >= facingY ? pushX : pushY;
	}
	else
	{
		//–Ê‚Ì“r’†‚Å‚ÍA¡‚Ü‚Å’Ê‚è‚»‚Ì–Ê‚Ì”½‘Î‘¤‚Ö‰Ÿ‚·
		const float sideX = FMath::Abs(localPosition.X) / FMath::Max(1.0, extent.X);
		const float sideY = FMath::Abs(localPosition.Y) / FMath::Max(1.0, extent.Y);
		m_moveDirection = sideX >= sideY ? pushX : pushY;
	}
	if (m_moveDirection.IsNearlyZero())
	{
		return;
	}

	m_startLocation = GetActorLocation();
	m_targetLocation = m_startLocation + m_moveDirection * m_moveDistance;
	m_moveProgress = 0.f;
	m_bIsMoving = true;
	SetActorTickEnabled(true);
}

//ƒCƒ“ƒ^ƒ‰ƒNƒg‚ÌÀ‘•iƒ|[ƒY‚·‚é‚©‚Ç‚¤‚©j
void APushMoveActor::Interact(AActor* _interactOwner, bool& _isPose)
{
	Interact(_interactOwner);
	_isPose = false;
}




//ƒCƒ“ƒ^ƒ‰ƒNƒg‚·‚éƒ|ƒWƒVƒ‡ƒ“‚ğæ“¾
FVector APushMoveActor::GetInteractPosition(const AActor* _actor /*= nullptr*/)
{
	return FVector::ZeroVector;
}


//ˆÚ“®ƒŠƒZƒbƒg
void APushMoveActor::ResetMove()
{
	m_bIsMoving = false;
	m_moveProgress = 0.f;
	SetActorLocation(m_initPos);
	SetActorTickEnabled(false);
}