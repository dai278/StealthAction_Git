//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "Shadow/ShadowComponent.h"
#include "Engine/World.h"
#include "Shadow/ShadowManager.h"
#include "Light/ExtendedSpotLight.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

//---------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//---------------------------------------
UShadowComponent::UShadowComponent()
	:m_lightIndex(-1)
	, m_pLight(nullptr)
	, m_shadowShape(EShadowShape::box)
	, m_pShadowMesh(nullptr)
	, m_pCollisionBox(nullptr)
	, m_scaleType(EShadowScaleType::OriginalScale)
	, m_isUsingAbsoluteRotation(true)
{
	//‰e‚ÌˆÊ’u‚ÆŒ`ó‚ğ–ˆƒtƒŒ[ƒ€XV‚·‚é
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(true);
	SetAutoActivate(true);

	//ƒƒbƒVƒ…ƒRƒ“ƒ|[ƒlƒ“ƒg¶¬
	m_pShadowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("shadowMesh"));
	if (m_pShadowMesh) {
		//ƒƒbƒVƒ…‚ÌƒRƒŠƒWƒ‡ƒ“‚ğOFF
		m_pShadowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//ƒI[ƒo[ƒ‰ƒbƒvƒCƒxƒ“ƒg‚Í”­¶‚³‚¹‚È‚¢
		m_pShadowMesh->SetGenerateOverlapEvents(false);
		m_pShadowMesh->SetVisibility(true);
		m_pShadowMesh->SetHiddenInGame(false);
	}

	//ƒRƒŠƒWƒ‡ƒ“
	m_pCollisionBox = CreateDefaultSubobject < UBoxComponent >(TEXT("m_pCollisionBox"));
	if (m_pCollisionBox) {

		//ƒRƒŠƒWƒ‡ƒ“‚ªd‚È‚Á‚Ä‚¢‚é‚©‚ÌŒŸ’m‚Ì‚İ
		m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//ƒRƒŠƒWƒ‡ƒ“‚Ì‘SƒOƒ‹[ƒv(ƒRƒŠƒWƒ‡ƒ“ƒ`ƒƒƒ“ƒlƒ‹)‚ğˆê“x‚·‚×‚Ä–³‹
		m_pCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		//‚»‚µ‚ÄƒvƒŒƒCƒ„[‚¾‚¯’Ç‰Á
		m_pCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		m_pCollisionBox->SetCollisionProfileName(TEXT("Shadow"));
		//‰¼ƒTƒCƒY
		m_pCollisionBox->SetBoxExtent(FVector{ 40.f,40.f,40.f });
		//ƒI[ƒoƒ‰ƒbƒvƒCƒxƒ“ƒg‚ğ”­¶‚³‚¹‚é‚©
		m_pCollisionBox->SetGenerateOverlapEvents(true);
	}	
}

//------------------------------------------
//BeginPlay‚ÌŒã‚ÉŒÄ‚Î‚ê‚éŠÖ”
//------------------------------------------
void UShadowComponent::OnRegister()
{
	Super::OnRegister();	

	if (m_pShadowMesh && m_pCollisionBox)
	{
		m_pCollisionBox->SetCollisionProfileName(TEXT("Shadow"));
	}
	SetActive(true);

	//ƒAƒ^ƒbƒ`
	if (m_pShadowMesh && m_pCollisionBox)
	{
		m_pShadowMesh->SetupAttachment(this);
		m_pCollisionBox->SetupAttachment(this);
	}

	//e‚Ì‰ñ“]‚É‰e‹¿‚³‚ê‚È‚¢‚æ‚¤‚É‚·‚é
	SetUsingAbsoluteRotation(true);

}


//--------------------------------------
// BeginPlay
//--------------------------------------
void UShadowComponent::BeginPlay()
{

	Super::BeginPlay();	

	//‰eƒ}ƒl[ƒWƒƒ[‚É“o˜^
	if (UWorld* World = GetWorld())
	{
		if (UShadowManager* Manager = World->GetSubsystem<UShadowManager>())
		{
			Manager->AddShadowComponent(this);
		}
	}

	//ƒI[ƒi[‚ÌZ²ƒXƒP[ƒ‹‚ğæ“¾
	//Z²‚ÍƒI[ƒi[‚ÌƒXƒP[ƒ‹‚ÉˆË‘¶‚µ‚È‚¢‚æ‚¤‚É‚·‚é
	//ƒI[ƒi‚ğæ“¾
	AActor* owner = GetOwner();

	if (!IsValid(owner)) { return; }

	//ƒƒbƒVƒ…ƒRƒŠƒWƒ‡ƒ“‚ÌƒXƒP[ƒ‹
	FVector newScale = GetComponentScale();
	//ƒI[ƒi[‚ÌƒXƒP[ƒ‹
	FVector ownerScale = owner->GetActorScale3D();
	 //‚š²‚Í‚PŒÅ’è
	 newScale.Z = 1.f;

	 //ƒ[ƒJƒ‹ƒXƒP[ƒ‹‚ğæ“¾
	 FVector localScale = GetRelativeScale3D();

	 //XY²‚ÌƒXƒP[ƒ‹‚ğ”½‰f
	 switch (m_scaleType)
	 {
	 case EShadowScaleType::OriginalScale:
		 //ƒIƒŠƒWƒiƒ‹‚Í‰½‚ğ‚µ‚È‚¢
	 {
		 break;
	 }
	 case EShadowScaleType::OwnerScaleX:
		 //X²‚ÌƒXƒP[ƒ‹‚ğ”½‰f
		 //Y²‚Í‚PŒÅ’è
	 {
		 newScale.Y = ownerScale.Z * 0.5f;
		 break;
	 }
	 case EShadowScaleType::OwnerScaleY:
	 {
		 //Y²‚ÌƒXƒP[ƒ‹‚ğ”½‰f
		 //X²‚Í‚PŒÅ’è
		 newScale.X = ownerScale.Z*0.5f;
		 break;
	 }
	}
	 //ƒ[ƒ‹ƒhƒTƒCƒY‚ğ•â³
	 SetWorldScale3D(newScale);	
}

//--------------------------------------
//XVˆ—
//--------------------------------------
void UShadowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateTransform();
}


//--------------------------------------
//ƒI[ƒi[‚É‘Î‚µ‚ÄŠp“x‚È‚Ç‚ğ‡‚í‚¹‚é
//--------------------------------------
void UShadowComponent::UpdateTransform()
{
	AActor* owner = GetOwner();
	if (!IsValid(owner) || !m_pLight.IsValid())
	{
		return;
	}
	// ƒI[ƒi[‚ÌˆÊ’u
	FVector origin = owner->GetActorLocation();
	// ƒJƒvƒZƒ‹ƒRƒ“ƒ|[ƒlƒ“ƒg‚ª‚ ‚ê‚Îã’[‚É’²®
	if (UCapsuleComponent* capsule = owner->FindComponentByClass<UCapsuleComponent>())
	{
		const float halfHeight = capsule->GetScaledCapsuleHalfHeight();
		origin += FVector(0.f, 0.f, halfHeight); // ã’[
	}

	// ƒ‰ƒCƒgˆÊ’u
	const FVector lightPos = m_pLight->GetLightLocation();

	// ‹——£‚ªƒ‰ƒCƒg‚Ì“Í‚­”ÍˆÍŠO‚È‚çƒXƒLƒbƒv
	const float lightLength = m_pLight->GetLightLength();
	const float dist = FVector::Distance(origin, lightPos);
	if (lightLength > 0.f && dist > lightLength)
	{
		return;
	}

	// Œõ‚Ìis•ûŒü
	FVector lightToObj = (origin - lightPos);
	if (lightToObj.IsNearlyZero())
	{
		return;
	}
	lightToObj.Normalize();

	// ‰e•ûŒüiƒIƒuƒWƒFƒNƒg¨‰e‚ªL‚Ñ‚é•ûŒüj
	FVector shadowDir = -lightToObj;

	// ’n–ÊiXY•½–Êj‚Ö“Š
	shadowDir.Z = 0.f;

	// ^ã‚©^‰º‚¾‚Æ•ûŒü‚ª’è‚Ü‚ç‚È‚¢
	if (shadowDir.IsNearlyZero())
	{
		return;
	}
	//³‹K‰»
	shadowDir.Normalize();

	// Radians‚ğDegrees‚É•ÏŠ·
	const float yawDeg = FMath::RadiansToDegrees(FMath::Atan2(shadowDir.Y, shadowDir.X));

	// ƒ[ƒ‹ƒh‰ñ“]‚É•ÏŠ·
	SetWorldRotation(FRotator(0.f, yawDeg+90.f, 0.f));

}


//----------------------------------------
//ƒ‰ƒCƒg‚ğƒZƒbƒg
//----------------------------------------
void UShadowComponent::SetLight(const AExtendedSpotLight* _pLight)
{
	m_pLight = _pLight;
}



//----------------------------------------
//ƒ‰ƒCƒgƒCƒ“ƒfƒbƒNƒX‚Ìæ“¾
//----------------------------------------
int32 UShadowComponent::GetLightIndex()const
{
	return m_lightIndex;
}


