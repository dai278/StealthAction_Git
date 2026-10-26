//’S“–“úîà
// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword/SwordAttackComponent.h"
#include "Components/SphereComponent.h"
#include "Damage/IDamage.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

//ƒRƒ“ƒXƒgƒ‰ƒNƒ^
USwordAttackComponent::USwordAttackComponent()
	:m_damage(10)//‰¼
	, m_radius(50.0f)//‰¼
	, m_timer(0.0f)
	, m_swingEndTime(0.8f)//‰¼
	, m_swingEndCallback(nullptr)
	, m_swordCollision(nullptr)
	, m_bSneakKill(false)
	, m_bIsSwinging(false)
	, m_KnockBackValu(1000.f)
{
	//U‚Á‚Ä‚¢‚éŠÔ‚Ì‚İTick—LŒø‰»AÅ‰‚Í–³Œø‰»
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);
	
	//ƒRƒŠƒWƒ‡ƒ“¶¬
	m_swordCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SwordCollision"));
	if (!IsValid(m_swordCollision)) {
	}
	else
	{
		m_swordCollision->SetupAttachment(this);
	}
}


// BeginPlay
void USwordAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!IsValid(m_swordCollision)) { return; }


	// ‰ñ“]‚ğe‚Ì‰e‹¿
	SetUsingAbsoluteRotation(false);

	SetComponentTickEnabled(false);

	m_swordCollision->OnComponentBeginOverlap.AddDynamic(this, &USwordAttackComponent::OnSwordBeginOverlap);

	m_swordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_swordCollision->SetGenerateOverlapEvents(false);

	//debug
}

// OnRegister
void USwordAttackComponent::OnRegister()
{
	Super::OnRegister();

	AActor* owner = GetOwner();
	if (IsValid(m_swordCollision))
	{
		m_swordCollision->SetupAttachment(this);
	}

	bHiddenInGame = true;

}


// Tick
void USwordAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsValid(m_swordCollision)) { SwingEnd(); return; }


	FName name= m_swordCollision->GetCollisionProfileName();


	//ŠÔŒv‘ª
	m_timer += DeltaTime;
	//U‚èI‚í‚è”»’è
	if (m_timer>= m_swingEndTime)
	{
		//U‚èI‚í‚èˆ—
		SwingEnd();
	}
}


//------------------------------
//Õ“Ë‘ÎÛ‚Ì’Ç‰Á
//------------------------------
void USwordAttackComponent::SetProfileName(const FName _name)
{
	if (!IsValid(m_swordCollision)) { return; }
	m_swordCollision->SetCollisionProfileName(FName(_name));
}


//------------------------------
//Õ“Ë‘ÎÛ‚ÌƒNƒŠƒA
//------------------------------
void USwordAttackComponent::ClearCollisionObjectType()
{
	if (!IsValid(m_swordCollision)) { return; }
	m_swordCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
}


//------------------------------
//Œ•‚ÌUŒ‚”ÍˆÍ‚Ì‘å‚«‚³w’è
//------------------------------
void USwordAttackComponent::SetSwordAttackScale(const float& _radius)
{
	if (!IsValid(m_swordCollision)) { return; }

	m_swordCollision->SetSphereRadius(_radius);
}

//------------------------------
//‘±ŠÔİ’è
//------------------------------
void USwordAttackComponent::SetAttackTime(const float& _time)
{
	m_swingEndTime = _time;
}


//------------------------------
//Œ•‚ğU‚éiˆø”‚ ‚èj
//------------------------------
void USwordAttackComponent::Swinging(const int& _damage, const FVector& _location, const float& _radius, const float& _attackTime, const bool& _bSneakKill)
{
	if(m_bIsSwinging)
	{
		//‚·‚Å‚ÉU‚Á‚Ä‚¢‚é‚È‚ç–³‹
		return;
	}
	m_damage = _damage;
	m_radius = _radius;
	m_swingEndTime = _attackTime;
	bHiddenInGame = true;
	Swinging(_bSneakKill);
}

//------------------------------
//Œ•‚ğU‚éiˆø”‚È‚µj
//------------------------------
void USwordAttackComponent::Swinging(const bool& _bSneakKill)
{
	if (!IsValid(m_swordCollision)) { return; }
	
	if(m_bIsSwinging)
	{
		//‚·‚Å‚ÉU‚Á‚Ä‚¢‚é‚È‚ç–³‹
		return;
	}
	SetComponentTickEnabled(true);
	m_bIsSwinging = true;
	m_timer = 0.0f;
	m_bSneakKill = _bSneakKill;
	
	if (IsValid(m_swordCollision))
	{
		m_swordCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		m_swordCollision->SetGenerateOverlapEvents(true);
	}
}

//------------------------------
//Œ•‚ğU‚èI‚í‚ç‚¹‚éi”CˆÓƒ^ƒCƒ~ƒ“ƒOj
//------------------------------
void USwordAttackComponent::SwingEnd() 
{
	//U‚èI‚í‚èˆ—
	//Tick–³Œø‰»
	SetComponentTickEnabled(false);
	//”ñ•\¦‰»
	bHiddenInGame = false;
	//ƒ^ƒCƒ}[ƒŠƒZƒbƒg
	m_timer = 0.0f;
	//ƒXƒj[ƒNƒLƒ‹‚Å‚È‚¢ó‘Ô‚É–ß‚·
	m_bSneakKill=false;
	//Œ•‚ğU‚Á‚Ä‚¢‚éó‘Ô‚ğ‰ğœ
	m_bIsSwinging = false;
	//ƒRƒŠƒWƒ‡ƒ“–³Œø‰»
	if (IsValid(m_swordCollision))
	{
		m_swordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_swordCollision->SetGenerateOverlapEvents(false);
	}

	if(m_swingEndCallback)
	{
		//U‚èI‚í‚Á‚½‚±‚Æ‚ğ’Ê’m
		m_swingEndCallback();
	}
}

//------------------------------
//Œ•‚ÌUŒ‚”»’è—pƒRƒŠƒWƒ‡ƒ“‚ÆÕ“Ë‚µ‚½uŠÔ‚ÌƒCƒxƒ“ƒgŠÖ”
//------------------------------
void USwordAttackComponent::OnSwordBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AActor* Owner = GetOwner();
	if (!IsValid(OtherActor) || !IsValid(Owner) || OtherActor == Owner) { return; }
	IDamageable* Damageable = Cast<IDamageable>(OtherActor);
	if (!Damageable) { return; }


	//ƒmƒbƒNƒoƒbƒN
	FVector knockBackvector = OtherActor->GetActorLocation()-Owner->GetActorLocation() ;
	knockBackvector.Z = 0.f;
	knockBackvector = knockBackvector.GetSafeNormal();
	knockBackvector *= m_KnockBackValu;

	
	//ƒ_ƒ[ƒW—^‚¦‚éˆ—
	Damageable->OnDamage(m_damage, knockBackvector,m_bSneakKill);

}


//------------------------------
//Œ•‚ğU‚èI‚í‚Á‚½’Ê’m—pƒR[ƒ‹ƒoƒbƒN“o˜^
//------------------------------
void USwordAttackComponent::RegisterSwingEndCallback(const std::function<void()>& func)
{
	if (!func) { return ;}
	m_swingEndCallback = func;
}


