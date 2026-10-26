//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Light/ExtendedSpotLight.h"
#include "Engine/World.h"
#include "Components/SpotLightComponent.h"

#include "Light/ExtendedSpotLightManager.h"

//------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//------------------------------------------
AExtendedSpotLight::AExtendedSpotLight()
	: m_bCanEverTick(false)
	, m_lightIndex(-1)
	, m_pSpotLight(nullptr)
	, m_pMesh(nullptr)
	, m_isEnemy(false)
	, m_automaticRotateYawSpeed(30.f)
	, m_bAutomaticRotateYaw(false)
	,m_bAutomaticRotatePitch(false)
	, m_automaticRotatePitchSpeed(30.f)
	, m_bRotateTurn(true)
	, m_minTurnRotate(0.f)
	, m_maxTurnRotate(180.f)
	, m_turnDir(1)
	, m_bBlink(false)
	, m_blinkOffTime(2.f)
	, m_blinkOnTime(2.f)
	, m_canItBeTurned(false)
	, m_bIsOn(true)
	, m_blinkTimer(0.f)
	, m_bManualRotateing(false)
	, m_LightStatus(ELightStatus::None)
	, m_lightOffTimer(0.f)
	, m_LightOffTime(5.f)
	, m_bLightOff(false)
	, m_automaticRollYawSpeed(30.f)
{

	//–ˆƒtƒŒ[ƒ€XVˆ—‚ğs‚¤‚©.
	PrimaryActorTick.bCanEverTick = true;
	//ƒXƒ|ƒbƒgƒ‰ƒCƒg¶¬
	m_pSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	//Root‚ğì¬
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//ƒƒbƒVƒ…‚Ì¶¬
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	m_pMesh->SetupAttachment(RootComponent);
	//Root‚ÉSpotLight‚ğƒAƒ^ƒbƒ`
	m_pSpotLight->SetupAttachment(m_pMesh);
}


//------------------------------------------
// BeginPlayiƒQ[ƒ€ŠJnj
//------------------------------------------
void AExtendedSpotLight::BeginPlay()
{
	Super::BeginPlay();

	//BPã‚Åİ’è‚µ‚½–ˆƒtƒŒ[ƒ€XV‚·‚é‚©
	//“®‚©‚·ƒ‰ƒCƒg‚Í–ˆƒtƒŒ[ƒ€
	//PrimaryActorTick.bCanEverTick = m_bCanEverTick;

	//ƒ}ƒl[ƒWƒƒ[‚É“o˜^
	if (UWorld* World = GetWorld())
	{
		if (UExtendedSpotLightManager* Manager = World->GetSubsystem<UExtendedSpotLightManager>())
		{
			Manager->AddLight(this);
		}
	}
	if (!IsValid(m_pSpotLight)) { return; }
	//ƒCƒ“ƒfƒbƒNƒX‚ª“o˜^‚³‚ê‚Ä‚¢‚È‚¯‚ê‚Î“o˜^‚µ‚È‚¢
	if(m_lightIndex==-1){ m_pSpotLight->SetCastShadows(false); }
	//‰e‚ğo‚·
	else { m_pSpotLight->SetCastShadows(true); }

	//ƒ‰ƒCƒg‚ª‚Â‚¢‚Ä‚¢‚È‚¢İ’è‚É‚È‚Á‚Ä‚¢‚ê‚ÎÁ‚·
	if (!m_bIsOn) { m_pSpotLight->SetVisibility(false); }
}


//------------------------------------------
// XVˆ—
//------------------------------------------
void AExtendedSpotLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsValid(m_pSpotLight)) { return; }



	//// ƒXƒ|ƒbƒgƒ‰ƒCƒg‚ÌˆÊ’uiƒ[ƒ‹ƒhÀ•Wj
	//FVector Origin = m_pSpotLight->GetComponentLocation();

	//// ƒXƒ|ƒbƒgƒ‰ƒCƒg‚Ì•ûŒüiƒ[ƒ‹ƒhÀ•WŒn‚Ì‘O•ûƒxƒNƒgƒ‹ = ÆË•ûŒüj
	//FVector Direction = m_pSpotLight->GetForwardVector();

	//// ÆË‹——£iÆË”ÍˆÍ‚Ì’·‚³j
	//float Length = m_pSpotLight->AttenuationRadius;

	//// ‰~‚ÌŠp“xiOuterConeAnglej‚ğƒ‰ƒWƒAƒ“•ÏŠ·
	//float AngleWidth = FMath::DegreesToRadians(m_pSpotLight->OuterConeAngle);
	//float AngleHeight = AngleWidth; // ‰~‚È‚Ì‚Å“¯‚¶Šp“x‚ğg—p

	//// ”’F‚Å•`‰æ
	//FColor ConeColor = FColor::White;

	//// DrawDebugCone‚Ì•`‰æƒpƒ‰ƒ[ƒ^
	//DrawDebugCone(
	//	GetWorld(),      // ƒ[ƒ‹ƒh
	//	Origin,          // ‰~‚Ì”­¶“_
	//	Direction,       // ÆË•ûŒüƒxƒNƒgƒ‹
	//	Length,          // ‰~‚Ì’·‚³(‚‚³)
	//	AngleWidth,      // ‰~‚Ì…•½”¼Špiƒ‰ƒWƒAƒ“j
	//	AngleHeight,     // ‰~‚Ì‚’¼”¼Špiƒ‰ƒWƒAƒ“j
	//	16,              // •ªŠ„”iŠŠ‚ç‚©‚³j
	//	ConeColor,       // F
	//	false,           // ‰i‘±•`‰æ‚Å‚Í‚È‚¢i–ˆƒtƒŒ[ƒ€•`‰æ‚ÅXVj
	//	-1.f,            // ƒ‰ƒCƒtƒ^ƒCƒ€i-11ƒtƒŒ[ƒ€‚Ì‚İj
	//	0,               // [“x—Dæ“x
	//	2.f              // ü‚Ì‘¾‚³
	//);

	switch (m_LightStatus)
	{
	case ELightStatus::AutoYawRotate:
		UpdateAutoYawRotate(DeltaTime);
		break;
	case ELightStatus::AutoPitchRotate:
		UpdateAutoPitchRotate(DeltaTime);
		break;
	case ELightStatus::AutoRollRotate:
			UpdateAutoRollRotate(DeltaTime);
			break;
	case ELightStatus::ManualYawRotate:
		UpdateManualYawRotate(DeltaTime);
		break;
	case ELightStatus::ManualPitchRotat:
		UpdateManualPitchRotate(DeltaTime);
		break;
	case ELightStatus::Blink:
		UpdateBlink(DeltaTime);
		break;
	default:
		break;
	}


	if (m_bLightOff)
	{
		m_lightOffTimer += DeltaTime;
		if (m_lightOffTimer > m_LightOffTime)
		{
			TurnOn();
		}
	}
}

//------------------------------------------
//ƒ‰ƒCƒgƒCƒ“ƒfƒbƒNƒX‚Ìæ“¾
//------------------------------------------
int32 AExtendedSpotLight::GetIndex()const
{
	return m_lightIndex;
}

//------------------------------------------
//ƒ‰ƒCƒg‚ÌˆÊ’uæ“¾
//------------------------------------------
FVector AExtendedSpotLight::GetLightLocation()const
{
	if (!IsValid(m_pSpotLight)) { return GetActorLocation(); }

	return m_pSpotLight->GetComponentLocation();
}

//-----------------------------------
//ƒ‰ƒCƒg‚ÌŒü‚«æ“¾
//-----------------------------------
FVector AExtendedSpotLight::GetLightForwardVector()const
{
	if (!IsValid(m_pSpotLight)) { return GetActorForwardVector(); }

	return m_pSpotLight->GetForwardVector();
}

//---------ÆË”ÍˆÍ-----------

// --------------------------------
//‰~‚Ì“à‘¤Šp“x
//---------------------------------
float AExtendedSpotLight::GetLightInnerAngle()const
{
	if (!IsValid(m_pSpotLight)) { return 0.f; }

	return m_pSpotLight->InnerConeAngle;
}

//---------------------------------
//‰~ŠO‘¤Šp“x
//----------------------------------
float AExtendedSpotLight::GetLightOuterAngle()const
{
	if (!IsValid(m_pSpotLight)) { return 0.f; }

	return m_pSpotLight->OuterConeAngle;
}

//-----------------------------------
//Œõ‚Ì“Í‚­‹——£
//-----------------------------------
float AExtendedSpotLight::GetLightLength()const
{
	if (!IsValid(m_pSpotLight)) { return 0.f; }

	return m_pSpotLight->AttenuationRadius;
}

//-----------------------------------
//ƒ‰ƒCƒg‚ÌON OFFİ’è
//----------------------------------
void AExtendedSpotLight::SetVisibility(const bool isVisibility)
{
	if (!IsValid(m_pSpotLight)) { return; }

	m_pSpotLight->SetVisibility(isVisibility);
}

//----------------------------------
//ƒ‰ƒCƒg‚ª“®‚­‚©?
//----------------------------------
bool AExtendedSpotLight::IsMove()const
{
	return m_bCanEverTick;
}


//------------------------------
//ƒ‰ƒCƒg‚Æ‚ÌÕ“Ëƒ`ƒFƒbƒN
//------------------------------
bool AExtendedSpotLight::IsHit(const FVector& _pos)const
{
	if (!GetWorld()) { return false; }

	if (!IsValid(m_pSpotLight)) { return false; }

	if (!m_bIsOn) { return false; }

	FVector lightPos = m_pSpotLight->GetComponentLocation();
	FVector lightDir = m_pSpotLight->GetForwardVector();

	FVector ToPoint = (_pos - lightPos);
	//‹——£
	float distance = ToPoint.Size();

	// ‹——£ƒ`ƒFƒbƒN
	if (distance > m_pSpotLight->AttenuationRadius)
	{
		return false;
	}

	ToPoint.Normalize();

	// Šp“xƒ`ƒFƒbƒN
	float CosAngle = FVector::DotProduct(lightDir, ToPoint);
	float CosOuterCone = FMath::Cos(FMath::DegreesToRadians(m_pSpotLight->OuterConeAngle));
	//Šp“x‚É“ü‚Á‚Ä‚¢‚È‚¯‚ê‚Î
	if (CosAngle < CosOuterCone) { return false; }

	// Õ•Á•¨ƒ`ƒFƒbƒNi‰e”»’èj
	FHitResult Hit;
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	//“G‚àÕ•Á•¨‚Æ‚µ‚Ä”»’è‚·‚é
	ObjParams.AddObjectTypesToQuery(ECC_GameTraceChannel8);
	// Shadowê—pObjectType‚Íu“ü‚ê‚È‚¢v

	FCollisionQueryParams Params(SCENE_QUERY_STAT(LightOcclusion), false);
	Params.AddIgnoredActor(this);

	/* ‰e‚ª•¡”ƒRƒ“ƒ|[ƒlƒ“ƒg‚È‚ç‘S•” AddIgnoredComponent */
	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		lightPos,
		_pos,
		ObjParams,
		Params
	);

	if (bHit)
	{
		// ‚³‚¦‚¬‚ç‚ê‚¸A­‚µƒ}[ƒWƒ“
		if (Hit.Distance < distance)
		{
			return false;
		}
	}

	return true;

}



//-----------------------------------------------------
//ƒ‰ƒCƒg‚ğ•t‚¯‚éˆ—
//-----------------------------------------------------
void AExtendedSpotLight::TurnOn()
{
	if (!IsValid(m_pSpotLight)) { return; }

	if (m_canItBeTurned) {
		m_pSpotLight->SetVisibility(true);
		m_bIsOn = true;
		m_blinkTimer = 0.f;
		m_lightOffTimer = 0.f;

		m_bLightOff = false;

	}
}

//-----------------------------------------------------
//ƒ‰ƒCƒg‚ğÁ‚·
//-----------------------------------------------------
void AExtendedSpotLight::TurnOff()
{
	if (!IsValid(m_pSpotLight)) { return; }

	if (m_canItBeTurned) {
		m_pSpotLight->SetVisibility(false);
		m_bIsOn = false;
		m_blinkTimer = 0.f;

		if (m_LightStatus != ELightStatus::Blink)
		{
			m_bLightOff = true;
		}
	}
}

void AExtendedSpotLight::SwitchTurn()
{
	if (m_bIsOn)
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}


//------------------------------------------------------
//Å‘å’lÅ¬’l‚Ü‚Å‚Ì‰ñ“]
//------------------------------------------------------
void AExtendedSpotLight::OnRotateTargetAngleStop()
{
	m_bManualRotateing = true;
	//Œ»İ‚Ì‰ñ“]
	FRotator  currentRot = GetActorRotation();

	//if (m_minTurnRotate == currentRot.Pitch||m_minTurnRotate==currentRot.Yaw)
	//{
	//	m_turnDir = 1.f;
	//}
	//else if (m_maxTurnRotate == currentRot.Pitch || m_maxTurnRotate == currentRot.Yaw)
	//{
	//	m_turnDir = -1.f;
	//}
	//else
	//{
	//	m_turnDir = 1.f;
	//}
}

////-----------------------------------------------------
////EndPlay
////-----------------------------------------------------
//void AExtendedSpotLight::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//	//ƒ}ƒl[ƒWƒƒ[‚É“o˜^
//	UExtendedSpotLightManager* mng =GetWorld()->GetSubsystem<UExtendedSpotLightManager>();
//	if (mng)
//	{
//		mng->UnRegisterLight(this);
//	}
//}




//-----------------------------------------------------
//©“®YAw‰ñ“]‚ÌXVˆ—
//-----------------------------------------------------
void AExtendedSpotLight::UpdateAutoYawRotate(const float& _deltaTime)
{
	//FRotator newRotator = GetActorRotation();
	//
	//Œ»İ‚Ì‰ñ“]—Ê‚ªÅ‘å’lÅ¬’l‚Ì”ÍˆÍ“à‚Å‚È‚¯‚ê‚Î”ÍˆÍ“à‚Ö‰ñ“]
	//if (newRotator.Yaw< m_minTurnRotate )
	//{
	//	m_turnDir = 1;
	//}
	//else if (newRotator.Yaw > m_maxTurnRotate)
	//{
	//	m_turnDir = -1;
	//}
	//
	//‰ñ“]
	//newRotator.Yaw += m_automaticRotateYawSpeed * _deltaTime*m_turnDir;
	//SetActorRotation(newRotator);

	//if (!m_bRotateTurn) {
	//	return;
	//}
	//
	//Å‘å’l‚ğ’´‚¦‚½‚©
	//bool isOver=false;
	//Å‘å’l‚ğ’´‚¦‚½‚ç
	//if (newRotator.Yaw > m_maxTurnRotate)
	//{
	//	newRotator.Yaw = m_maxTurnRotate;
	//	isOver = true;
	//}
	//Å¬’l–¢–‚É‚È‚Á‚½‚ç
	//else if (newRotator.Yaw < m_minTurnRotate)
	//{
	//	newRotator.Yaw = m_minTurnRotate;
	//	isOver = true;
	//}	

	//SetActorRotation(newRotator);
	//’´‚¦‚½Û‚Ìˆ—
	//if (isOver)
	//{
	//		m_turnDir *= -1.f;
	//}

	FRotator Rot = GetActorRotation();

	Rot.Yaw += m_automaticRotatePitchSpeed * _deltaTime * m_turnDir;

	if (Rot.Yaw >= m_maxTurnRotate)
	{
		Rot.Yaw = m_maxTurnRotate;
		m_turnDir = -1;
	}
	else if (Rot.Yaw <= m_minTurnRotate)
	{
		Rot.Yaw = m_minTurnRotate;
		m_turnDir = 1;
	}

	SetActorRotation(Rot);


}

//-----------------------------------------------------
//©“®Pitch‰ñ“]‚ÌXVˆ—
//-----------------------------------------------------
void AExtendedSpotLight::UpdateAutoPitchRotate(const float& _deltaTime)
{

		FRotator Rot = GetActorRotation();

		Rot.Pitch += m_automaticRotatePitchSpeed * _deltaTime * m_turnDir;

		if (Rot.Pitch >= m_maxTurnRotate)
		{
			Rot.Pitch = m_maxTurnRotate;
			m_turnDir = -1;
		}
		else if (Rot.Pitch <= m_minTurnRotate)
		{
			Rot.Pitch = m_minTurnRotate;
			m_turnDir = 1;
		}

		SetActorRotation(Rot);


}


//--------------------------------------------------
//‰ñ“]ƒ[ƒ‹
//---------------------------------------------------
void AExtendedSpotLight::UpdateAutoRollRotate(const float& _deltaTime)
{
	FRotator Rot = GetActorRotation();

	Rot.Roll += m_automaticRotatePitchSpeed * _deltaTime * m_turnDir;

	if (Rot.Roll >= m_maxTurnRotate)
	{
		Rot.Roll = m_maxTurnRotate;
		m_turnDir = -1;
	}
	else if (Rot.Roll <= m_minTurnRotate)
	{
		Rot.Roll = m_minTurnRotate;
		m_turnDir = 1;
	}

	SetActorRotation(Rot);

}



//-----------------------------------------------------
//è“®‰ñ“]
//-----------------------------------------------------
void AExtendedSpotLight::UpdateManualYawRotate(const float& _deltaTime)
{

	if (!m_bManualRotateing) { return; }
	FRotator newRotator = GetActorRotation();

	//‰Šú‚ª‚Ì‰ñ“]—Ê‚ªÅ‘å’lÅ¬’l‚Ì”ÍˆÍ“à‚Å‚È‚¯‚ê‚Î”ÍˆÍ“à‚Ö‰ñ“]
	if (newRotator.Yaw < m_minTurnRotate)
	{
		m_turnDir = 1;
		newRotator.Yaw = m_minTurnRotate;

	}
	else if (newRotator.Yaw > m_maxTurnRotate)
	{
		m_turnDir = -1;
		newRotator.Yaw = m_maxTurnRotate;

	}

	//‰ñ“]
	newRotator.Yaw += m_automaticRotateYawSpeed * _deltaTime * m_turnDir;
	//Å‘å’l‚ğ’´‚¦‚½‚©
	bool isOver = false;
	//Å‘å’l‚ğ’´‚¦‚½‚ç
	if (newRotator.Yaw > m_maxTurnRotate)
	{
		newRotator.Yaw = m_maxTurnRotate;
		m_turnDir = -1;
		isOver = true;
	}
	//Å¬’l–¢–‚É‚È‚Á‚½‚ç
	else if (newRotator.Yaw < m_minTurnRotate)
	{
		newRotator.Yaw = m_minTurnRotate;
		m_turnDir = 1;
		isOver = true;
	}
	//’´‚¦‚½Û‚Ìˆ—
	if (isOver)
	{
		m_bManualRotateing = false;
	}

	SetActorRotation(newRotator);

}

//-----------------------------------------------------
//è“®‰ñ“]
//-----------------------------------------------------
void AExtendedSpotLight::UpdateManualPitchRotate(const float& _deltaTime)
{

	if (!m_bManualRotateing) { return; }
	FRotator newRotator = GetActorRotation();

	//‰ñ“]
	newRotator.Pitch += m_automaticRotateYawSpeed * _deltaTime * m_turnDir;
	SetActorRotation(newRotator);

	//Œ»İ‚Ì‰ñ“]—Ê‚ªÅ‘å’lÅ¬’l‚Ì”ÍˆÍ“à‚Å‚È‚¯‚ê‚Îˆ—‚µ‚È‚¢
	if (newRotator.Pitch < m_minTurnRotate)
	{
		m_turnDir = 1;
	}
	else if (newRotator.Pitch > m_maxTurnRotate)
	{
		m_turnDir = -1;
	}
	//‰ñ“]
	newRotator.Pitch += m_automaticRotatePitchSpeed * _deltaTime * m_turnDir;
	//Å‘å’l‚ğ’´‚¦‚½‚©
	bool isOver = false;
	//Å‘å’l‚ğ’´‚¦‚½‚ç
	if (newRotator.Pitch > m_maxTurnRotate)
	{
		newRotator.Pitch = m_maxTurnRotate;
		isOver = true;
	}
	//Å¬’l–¢–‚É‚È‚Á‚½‚ç
	else if (newRotator.Pitch < m_minTurnRotate)
	{
		newRotator.Pitch = m_minTurnRotate;
		isOver = true;
	}

	//’´‚¦‚½Û‚Ìˆ—
	if (isOver)
	{
		m_bManualRotateing = false;
	}

	SetActorRotation(newRotator);
}


//----------------------------------------------------
//“_–Å
//----------------------------------------------------
void AExtendedSpotLight::UpdateBlink(const float& _deltaTime)
{
	if (!m_bBlink) { return; }

	//ƒ^ƒCƒ}[‰ÁZ
	m_blinkTimer += _deltaTime;

	//‚Â‚¢‚Ä‚¢‚ÄŠÔ‚ğ’´‚¦‚Ä‚¢‚ê‚Î
	if (m_bIsOn && m_blinkTimer > m_blinkOnTime) {
		//ƒ‰ƒCƒg‚ğÁ‚·
		TurnOff();
	}
	else if (!m_bIsOn && m_blinkTimer > m_blinkOffTime)
	{
		//ƒ‰ƒCƒg‚ğ•t‚¯‚é
		TurnOn();
	}
}

