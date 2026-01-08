//----------------------------------------------------------
// 概要				：エネミー1を制御するオブジェクト
// 更新日　　　　 　：
// 担当				：24CU0237廣川菖
//----------------------------------------------------------


#include "Enemy/Enemy1.h"

#include "Enemy_Weapon/Enemy_Bullet/Enemy_Bullet_1.h"			//銃弾発射用
#include "Enemy_Weapon/Enemy_Weapon_1.h"	
#include "Light/ExtendedSpotLight.h"							//ライト情報を取得するため
#include "EnemyManager/EnemyManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sword/SwordAttackComponent.h"							//剣用

//----------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------
AEnemy1::AEnemy1()
{

}

//------------------------------------------------------------------------------------------------------------
// ゲームスタート時、または生成時に呼ばれる
//------------------------------------------------------------------------------------------------------------
void AEnemy1::BeginPlay()
{
	Super::BeginPlay();

	//
	if (m_pEnemy_Weapon)
	{
		m_pEnemy_Weapon->SetOwner(this);
	}


	// ライトBPクラスが設定されていなければ何もしない
	if (!m_spotLightClass) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 生成（初期位置は敵の位置）
	m_spotLightInstance = GetWorld()->SpawnActor<AExtendedSpotLight>(
		m_spotLightClass,
		GetActorLocation(),
		GetActorRotation(),
		Params
	);

	if (!m_spotLightInstance) return;

	// 敵にアタッチ（敵のルートにくっつける）
	m_spotLightInstance->AttachToComponent(
		GetRootComponent(),
		FAttachmentTransformRules::KeepWorldTransform
	);

	m_spotLightInstance->SetActorRotation(FRotator{ -110.,0.,0. });
	m_spotLightInstance->SetActorLocation(m_spotLightInstance->GetActorLocation() + FVector{ 0.,0.,50. });

	//エネミーマネージャー登録
	UEnemyManager* enemyManager = GetWorld()->GetSubsystem<UEnemyManager>();
	if (enemyManager)
	{
		enemyManager->RegisterEnemy(this);
	}


	//
	if (m_sword)
	{
		//
		m_sword->ClearCollisionObjectType();
		//
		m_sword->SetProfileName(FName("EnemyAttack"));
		//
		m_sword->SetSwordAttackScale(50.0);
		//
		m_sword->SetAttackTime(1.0);
		//
		m_sword->RegisterSwingEndCallback(CreateSwingEndCallback(AEnemyBase::OnAttackEnd));
	}

	//
	m_enemyPos_Return = GetActorLocation();

}

