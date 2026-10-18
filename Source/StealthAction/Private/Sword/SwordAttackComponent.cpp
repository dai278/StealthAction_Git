// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword/SwordAttackComponent.h"
#include "Components/SphereComponent.h"
#include "Damage/IDamage.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

//コンストラクタ
USwordAttackComponent::USwordAttackComponent()
	:m_damage(10)//仮
	, m_radius(50.0f)//仮
	, m_timer(0.0f)
	, m_swingEndTime(0.8f)//仮
	, m_swingEndCallback(nullptr)
	, m_swordCollision(nullptr)
	, m_bSneakKill(false)
	, m_bIsSwinging(false)
	, m_KnockBackValu(1000.f)
{
	//振っている間のみTick有効化、最初は無効化
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);
	
	//コリジョン生成
	m_swordCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SwordCollision"));
	if (!IsValid(m_swordCollision)) {
		////UE_LOG(LogTemp, Display, TEXT("剣コリジョン生成失敗"));
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


	// 回転を親の影響
	SetUsingAbsoluteRotation(false);

	SetComponentTickEnabled(false);

	m_swordCollision->OnComponentBeginOverlap.AddDynamic(this, &USwordAttackComponent::OnSwordBeginOverlap);

	m_swordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_swordCollision->SetGenerateOverlapEvents(false);

	//debug
	//UE_LOG(LogTemp, Warning, TEXT("SwordCollision Registered=%d"), m_swordCollision->IsRegistered());
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

	//UE_LOG(LogTemp, Display, TEXT(""),name);

	//時間計測
	m_timer += DeltaTime;
	//振り終わり判定
	if (m_timer>= m_swingEndTime)
	{
		//振り終わり処理
		SwingEnd();
	}
}


//------------------------------
//衝突対象の追加
//------------------------------
void USwordAttackComponent::SetProfileName(const FName _name)
{
	if (!IsValid(m_swordCollision)) { return; }
	m_swordCollision->SetCollisionProfileName(FName(_name));
}


//------------------------------
//衝突対象のクリア
//------------------------------
void USwordAttackComponent::ClearCollisionObjectType()
{
	if (!IsValid(m_swordCollision)) { return; }
	m_swordCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
}


//------------------------------
//剣の攻撃範囲の大きさ指定
//------------------------------
void USwordAttackComponent::SetSwordAttackScale(const float& _radius)
{
	if (!IsValid(m_swordCollision)) { return; }

	m_swordCollision->SetSphereRadius(_radius);
}

//------------------------------
//持続時間設定
//------------------------------
void USwordAttackComponent::SetAttackTime(const float& _time)
{
	m_swingEndTime = _time;
}


//------------------------------
//剣を振る（引数あり）
//------------------------------
void USwordAttackComponent::Swinging(const int& _damage, const FVector& _location, const float& _radius, const float& _attackTime, const bool& _bSneakKill)
{
	if(m_bIsSwinging)
	{
		//すでに振っているなら無視
		return;
	}
	m_damage = _damage;
	m_radius = _radius;
	m_swingEndTime = _attackTime;
	bHiddenInGame = true;
	Swinging(_bSneakKill);
}

//------------------------------
//剣を振る（引数なし）
//------------------------------
void USwordAttackComponent::Swinging(const bool& _bSneakKill)
{
	if (!IsValid(m_swordCollision)) { return; }
	
	if(m_bIsSwinging)
	{
		//すでに振っているなら無視
		return;
	}
	//UE_LOG(LogTemp, Display, TEXT("KenHutta"));
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
//剣を振り終わらせる（任意タイミング）
//------------------------------
void USwordAttackComponent::SwingEnd() 
{
	//振り終わり処理
	//Tick無効化
	SetComponentTickEnabled(false);
	//非表示化
	bHiddenInGame = false;
	//タイマーリセット
	m_timer = 0.0f;
	//スニークキルでない状態に戻す
	m_bSneakKill=false;
	//剣を振っている状態を解除
	m_bIsSwinging = false;
	//コリジョン無効化
	if (IsValid(m_swordCollision))
	{
		m_swordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_swordCollision->SetGenerateOverlapEvents(false);
	}

	if(m_swingEndCallback)
	{
		//振り終わったことを通知
		m_swingEndCallback();
	}
}

//------------------------------
//剣の攻撃判定用コリジョンと衝突した瞬間のイベント関数
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

	//UE_LOG(LogTemp, Display, TEXT("NanikaniHit"));

	//ノックバック
	FVector knockBackvector = OtherActor->GetActorLocation()-Owner->GetActorLocation() ;
	knockBackvector.Z = 0.f;
	knockBackvector = knockBackvector.GetSafeNormal();
	knockBackvector *= m_KnockBackValu;

	
	//UE_LOG(LogTemp, Display, TEXT("Hit to Enemy"));
	//ダメージ与える処理
	Damageable->OnDamage(m_damage, knockBackvector,m_bSneakKill);

}


//------------------------------
//剣を振り終わった通知用コールバック登録
//------------------------------
void USwordAttackComponent::RegisterSwingEndCallback(const std::function<void()>& func)
{
	if (!func) { return ;}
	m_swingEndCallback = func;
}


