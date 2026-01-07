//----------------------------------------------------------
// 概要				：エネミー1を制御するオブジェクト
// 更新日　　　　 　：
// 担当				：24CU0237廣川菖
//----------------------------------------------------------


#include "Enemy/EnemyBase.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"		//プレイヤーも情報を参照するため
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h" 

#include "GameFramework/CharacterMovementComponent.h"

#include "Noise/NoiseManager.h"									//物音を取得するため
#include "Noise/NoiseListenerComponent.h"

#include "AIController.h"										//NavMeshを扱うため
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Navigation/PathFollowingComponent.h"

#include "Shadow/ShadowComponent.h"								//影を描画するため

#include "Enemy_Route/Enemy_RouteManager.h"						//ルート検索用

#include "Enemy_Weapon/Enemy_Bullet/Enemy_Bullet_1.h"			//銃弾発射用
#include "Enemy_Weapon/Enemy_Weapon_1.h"	

#include "Light/ExtendedSpotLight.h"							//ライト情報を取得するため

#include "EnemyManager/EnemyManager.h"
#include "Enemy/EnemyBase.h"

#include "Sword/SwordAttackComponent.h"							//剣用
//-----------------------------------------------------------
//検証用
//-----------------------------------------------------------
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"


#include "Enemy_Weapon/Enemy_Weapon_1.h"			

#include "Enemy_Weapon/Enemy_Bullet/Enemy_Bullet_1.h"
#include "Enemy_Weapon/Enemy_Weapon_1.h"
#include "Enemy_Weapon/Enemy_Bullet/Enemy_BulletStorage_1.h"

//----------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------
AEnemyBase::AEnemyBase()
	: m_pPlayerChara(NULL)
	, m_visiblityAngle(0.2)
	, m_visionLevel(0)
	, m_visionRange_Short(1000.0)
	, m_visionRange_Normal(1800.0)
	, m_visionRange_Long(2500.0)
	, m_hearingRange_Short(1500.0)
	, m_hearingRange_Normal(2300.0)
	, m_hearingRange_Long(3000.0)
	, m_stopDistance_Noise(50.0) //
	, m_patrolTime(0.0)
	, m_doubtTime(0.0)
	, m_doubtNoiseTime(0.0)
	, m_cautionTime(0.0)
	, m_cautionNoiseTime(0.0)
	, m_battleTime(0.0)
	, m_battleFalseTime(0.0)
	, m_battleNoiseTime(0.0)
	, m_alertTime(0.0)
	, m_missTime(0.0)
	, m_returnTime(0.0)
	, m_hearingTime(0.0)
	, m_patrolTime_Limit(2.0)
	, m_patrol_TurningTime_Limit(0.5f)
	, m_patrol_TurningCheckingTime_Limit(1.5f)
	, m_doubtTime_Limit(4.0)
	, m_doubtNoiseTime_Limit(2.0)
	, m_cautionTime_Limit(2.0)
	, m_cautionNoiseTime_Limit(2.0)
	, m_battleTime_Limit(1.0)
	, m_battleFalseTime_Limit(0.5)
	, m_battleNoiseTime_Limit(2.0)
	, m_alertTime_Limit(10.0)
	, m_missTime_Limit(1.0)
	, m_returnTime_Limit(2.0)
	, m_hearingTime_Limit(2.0)
	, m_attackingTime_Limit(0.2)
	, m_chaseSpeed_Slow(200.0f)
	, m_chaseSpeed_Normal(300.0f)
	, m_chaseSpeed_Fast(400.0f)
	, m_chaseRotSpeed(6.f)
	, m_hitDamage(5)
	, m_stopDistance_Player(150.0)
	, m_stopDistance_2D(50.0)
	, m_stopDistance_Nav(0.0)
	, m_attackDistance(1500.0)
	, m_patrolCancel(false)
	, m_moveStop_Nav(false)
	, m_visionCheck(false)
	, m_alertCheck(false)
	, m_patrolCheck(false)
	, m_doubtCheck(false)
	, m_doubtNoiseCheck(false)
	, m_cautionCheck(false)
	, m_cautionNoiseCheck(false)
	, m_battleCheck(false)
	, m_battleNoiseCheck(false)
	, m_missCheck(false)
	, m_returnCheck(false)
	, m_patrol_TurningCheck(false)
	, m_patrol_TurnDirection(0)
	, m_stopDistance_Wall(400.0)
	, m_patrol_TurningCheckingTime(0.f)
	, m_noiseCheck(false)
	, m_noise_Pos(-5000., -5000., -5000.)
	, m_playerPos(0., 0., 0.)
	, m_playerPos_LastSeen(0., 0., 0.)
	, m_playerPos_Nav_LastSeen(0., 0., 0.)
	, m_noise_Pos_keeper(-5000., -5000., -5000.)
	, m_noiseLevel(0)
	, m_noiseLevel_keeper(0)
	, m_enemyPos_Forward_Miss(0., 0., 0.)
	, m_enemyPos_Left_Miss(0., 0., 0.)
	, m_enemyPos_Right_Miss(0., 0., 0.)
	, m_enemyPos_Direction_Miss(0., 0., 0.)
	, m_enemyPos_Return(0., 0., 0.)
	, m_enemyDirection_Return(0., 0., 0.)
	, m_playerShadowCheck(false)
	, m_enemyCurrentState(EEnemyStatus::Patrol)
	, m_enemyCurrentState_Check(EEnemyStatus::Empty)
	, m_enemyCurrentState_Keeper(EEnemyStatus::Empty)
	, m_randomRoute(false)
	, m_routeNum(0)
	, m_routeCounter(0)
	, m_deadCheck(false)
	, m_allTime(0)
	, IsUseVisiblity(true)
	, IsUseHearing(true)
{
	// 毎フレーム、このクラスのTick()を呼ぶかどうかを決めるフラグ
	PrimaryActorTick.bCanEverTick = true;

	//デバック用
	noise = CreateDefaultSubobject<UNoiseListenerComponent>(TEXT("NoiseListener"));
	if (noise)
	{
		UE_LOG(LogTemp, Display, TEXT("ノイズリスナー生成完了"));
		noise->SetCallBackFunction(CreateNoiseEventCallback(AEnemyBase::OnNoiseHeard));
		noise->SetActive(true);
	}

	// AIController を使わせる
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	//影オブジェクトの生成
	m_pShadow = CreateDefaultSubobject<UShadowComponent>(TEXT("Shadow"));
	if (m_pShadow) {
		// m_pShadow->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		m_pShadow->SetupAttachment(GetRootComponent());
		m_pShadow->SetRelativeLocation(FVector::ZeroVector);
		m_pShadow->SetRelativeRotation(FRotator::ZeroRotator);
	}

	//
	m_sword = CreateDefaultSubobject<USwordAttackComponent>(TEXT("Sword"));

	if (m_sword)
	{
		m_sword->SetupAttachment(GetMesh());
		m_sword->SetRelativeLocation(FVector::ZeroVector);
		m_sword->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

//------------------------------------------------------------------------------------------------------------
// ゲームスタート時、または生成時に呼ばれる
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// 入力を有効化（検証用）
	EnableInput(GetWorld()->GetFirstPlayerController());


	UE_LOG(LogTemp, Warning, TEXT("AEnemyBase BeginPlay"));

	//ゲーム全体び対するActorの検索処理はコストが高いため、BeingPlayで一回保存しておくだけにする
	// 
	m_pPlayerChara = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (!m_pPlayerChara)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find PlayerChara"));
	}


	//検索対象は全てActor
	TSubclassOf<AActor> findClass;
	findClass = AActor::StaticClass();
	TArray<AActor*> EnemyBase_Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyBase::StaticClass(), EnemyBase_Actors);

	for (AActor* actor : EnemyBase_Actors)
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(actor);
		if (enemy && enemy != this)
		{
			// 自分以外の敵
			m_pOtherEnemyBase.Add(enemy);
		}
	}

	TArray<AActor*> Enemy_Bullet_1_Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemy_Bullet_1::StaticClass(), Enemy_Bullet_1_Actors);

	for (AActor* actor : Enemy_Bullet_1_Actors)
	{
		AEnemy_Bullet_1* bullet = Cast<AEnemy_Bullet_1>(actor);
		if (bullet)
		{
			// 自分以外の敵
			m_pALLBullet_1.Add(bullet);
		}
	}

	//カプセルコンポーネントとヒットした時に呼ばれるイベント関数を登録
	this->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyBase::OnHit);


	UCapsuleComponent* capsel = GetCapsuleComponent();
	//イベントを発生させるかtrue
	capsel->SetGenerateOverlapEvents(true);

	//コリジョンプリセットをプレイヤー
	capsel->SetCollisionProfileName(TEXT("Enemy"));

	m_enemyCurrentState = EEnemyStatus::Patrol;		//初めのステータスを巡回に設定

	//旋回速度
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	//NULLチェック
	UEnemy_RouteManager* RouteManager = GetWorld()->GetSubsystem<UEnemy_RouteManager>();
	if (!RouteManager)
	{
		UE_LOG(LogTemp, Error, TEXT("RouteManager is NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RouteManager Found"));
		m_pEnemy_Route = RouteManager->AddRoute(m_routeNum, m_randomRoute);
	}

	//ポインタを入力
	m_pEnemy_Route = GetWorld()->GetSubsystem<UEnemy_RouteManager>()->AddRoute(m_routeNum, m_randomRoute);

	//コメントアウトなおしたら治ったよ
	//AActor* Weapon = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemy_Weapon_1::StaticClass());
	//m_pEnemy_Weapon = Cast<AEnemy_Weapon_1>(Weapon);
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

	//レイを飛ばして敵とプレイヤーの間に遮蔽物がないかを確認
	//コリジョン判定で無視する項目を指定（今回Visiblityの場合）	//
	DefaultCollisionParams.AddIgnoredActor(m_pPlayerChara);
	DefaultCollisionParams.AddIgnoredActor(this);
	DefaultCollisionParams.AddIgnoredActor(m_pEnemy_Weapon);
	for (AEnemy_Bullet_1* bullet : m_pALLBullet_1)
	{
		DefaultCollisionParams.AddIgnoredActor(bullet);
	}

	//				
	BattleCollisionParams.AddIgnoredActor(this);
	BattleCollisionParams.AddIgnoredActor(m_pEnemy_Weapon);
	for (AEnemy_Bullet_1* bullet : m_pALLBullet_1)
	{
		BattleCollisionParams.AddIgnoredActor(bullet);
	}

}

//------------------------------------------------------------------------------------------------------------
// 各入力関係メソッドとのバインド処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//------------------------------------------------------------------------------------------------------------
// 毎フレームの更新処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ===== 検証用：Kキーで強制死亡 =====
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->WasInputKeyJustPressed(EKeys::K))
	{
		UE_LOG(LogTemp, Warning, TEXT("[[]]K Pressed"));
		m_enemyInfo.hp = 0;
	}
	//死亡判定
	if (m_enemyInfo.hp <= 0)
	{
		m_enemyCurrentState = EEnemyStatus::Dead;
		/*CaseDead(DeltaTime);
		return;*/
	}

	//------------------------------------------------------------
   // 死亡状態の場合は死亡処理のみ行う
   //------------------------------------------------------------
	if (m_enemyCurrentState == EEnemyStatus::Dead)
	{
		CaseDead(DeltaTime);
		return;
	}

	//初めの処理
	StartStateValues(DeltaTime);

	//視界処理
	UpdateVisiblity(DeltaTime);

	//聴覚処理
	UpdateHearing(DeltaTime);

	//探索処理
	UpdateSearch(DeltaTime);

	//警戒の処理
	UpdateAlert(DeltaTime);

	//リセット処理
	ResetStateValues(DeltaTime);

	//ステータス管理処理
	UpdateStatus(DeltaTime);
}

//------------------------------------------------------------------------------------------------------------
//ステータスが死亡の場合の処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseDead(float _deltaTime)
{
	// 初回のみ実行
	if (!m_deadCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("[[]]Dead"));

		m_deadCheck = true;

		// 移動停止
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();

		// Nav移動停止
		m_moveStop_Nav = true;

		// 判定系を全て止める
		m_searchStopper = true;
		m_hearingStopper = true;

		m_visionCheck = false;
		m_noiseCheck = false;

		// 各状態フラグを全OFF
		m_alertCheck = false;
		m_patrolCheck = false;
		m_doubtCheck = false;
		m_doubtNoiseCheck = false;
		m_cautionCheck = false;
		m_cautionNoiseCheck = false;
		m_battleCheck = false;
		m_battleNoiseCheck = false;
		m_missCheck = false;
		m_returnCheck = false;

		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 回転しないように
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

//------------------------------------------------------------------------------------------------------------
//初めの処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::StartStateValues(float _deltaTime)
{
	m_enemyPos = GetActorLocation();								//エネミーの座標
	m_enemyForward = GetActorForwardVector();						//エネミーの正面ベクトル
	m_enemyRight = GetActorRightVector();							//エネミーの右ベクトル
	m_playerPos = m_pPlayerChara->GetActorLocation();		//プレイヤーの座標

	//プレイヤーが影に入っているか？
	m_playerShadowCheck = m_pPlayerChara->IsInShadow();

	m_deltaTime = _deltaTime;
	m_allTime += _deltaTime;
}

//------------------------------------------------------------------------------------------------------------
//視界処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateVisiblity(float _deltaTime)
{
	//プレイヤーキャラが見つからなければ終了
	if (!m_pPlayerChara) { return; }

	if (!IsUseVisiblity)
	{
		m_visionCheck = false;		//視界OF
		return;
	}

	double distance = (m_playerPos - m_enemyPos).Length();			//プレイヤーとの距離を測る(Vectorの長さ）


	//プレイヤーが探索距離に入っていなければ、探索失敗で終了
	if (distance > m_visionRange_Long)
	{
		m_visionCheck = false;
		return;
	}

	FVector Target_Vector_V = (m_playerPos - m_enemyPos).GetSafeNormal();			//エネミーからプレイヤーへのベクトル

	float View_Value = FVector::DotProduct(m_enemyForward, Target_Vector_V);		//エネミー正面からプレイヤーへの角度

	//プレイヤーが視界内に入っていなければ、探索失敗で終了
	if (View_Value < m_visiblityAngle)
	{
		m_visionCheck = false;
		return;
	}

	//レイを飛ばして敵とプレイヤーの間に遮蔽物がないかを確認
	//コリジョン判定で無視する項目を指定（今回は敵キャラクター自身（this)）
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(m_pEnemy_Weapon);
	for (AEnemyBase* enemy : m_pOtherEnemyBase)
	{
		CollisionParams.AddIgnoredActor(enemy);
	}

	FHitResult HitCollision;		//ヒットした（＝コリジョン判定を受けた）オブジェクトを格納する変数

	//レイを飛ばし、全てのオブジェクトに対してコリジョン判定を行う
	bool isHit = GetWorld()->LineTraceSingleByObjectType(HitCollision, m_enemyPos, m_playerPos, FCollisionObjectQueryParams::AllObjects, CollisionParams);

	//ヒットするオブジェクトがある場合
	if (isHit)
	{
		AActor* hitActor = HitCollision.GetActor();

		//当たったコリジョンがプレイヤー以外だった場合
		if (!hitActor->ActorHasTag("Player"))
		{
			//探索失敗で終了
			m_visionCheck = false;
			return;
		}
	}
	else
	{
		//探索失敗で終了
		m_visionCheck = false;
		return;
	}

	//プレイヤーが影に入っているか？
	m_playerShadowCheck = m_pPlayerChara->IsInShadow();

	//入っている場合
	if (m_playerShadowCheck)
	{
		//探索失敗で終了
		m_visionCheck = false;
		return;
	}

	m_visionCheck = true;			//視界内にプレイヤーが映っている

	m_playerPos_LastSeen = m_playerPos;		//見えているときの座標を取得

	//探索レベルの設定
	//視界範囲（良）の場合
	if (distance <= m_visionRange_Short)
	{
		m_visionLevel = 4;
	}
	//視界範囲（普）の場合
	else if (distance <= m_visionRange_Normal)
	{
		m_visionLevel = 3;
	}
	//視界範囲（不）の場合
	else if (distance <= m_visionRange_Long)
	{
		m_visionLevel = 2;
	}

	//警戒状態の場合
	if (m_alertCheck)
	{
		m_visionLevel += 1;
	}

	//優先順位
	if (m_visionLevel == 4 || m_visionLevel == 5)
	{
		m_cautionCheck = false;
	}
}

//------------------------------------------------------------------------------------------------------------
//聴覚処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateHearing(float _deltaTime)
{
	if (!IsUseHearing)
	{
		m_noiseCheck = false;		//聴覚OF
		return;
	}

	//
	if (m_battleCheck)
	{
		m_noiseCheck = false;		//物音チェックOF
		return;
	}

	double distance = (m_noise_Pos - m_enemyPos).Length();			//物音との距離を測る(Vectorの長さ）
	double distance_keeper = (m_noise_Pos_keeper - m_enemyPos).Length();			//以前の物音との距離を測る(Vectorの長さ）

	//物音が遠くでした場合
	if (!m_noiseCheck && distance > m_hearingRange_Long)
	{
		m_noiseCheck = false;		//物音チェックOF
		return;
	}

	//物音が出たはじめのみ
	if (!m_noiseCheck && m_noiseLevel_keeper < 6)
	{
		m_noiseLevel_keeper = 0;//物音レベルをリセット
		m_hearingTime = 0;		//聴覚時間リセット
		m_noiseCheck = true;		//物音チェックON
	}

	m_hearingTime = _deltaTime;	//聴覚時間の更新

	//音の更新
	if (m_hearingTime < m_hearingTime_Limit)
	{
		//以前の物音が今の物音より小さい場合
		if (m_noiseLevel_keeper <= m_noiseLevel)
		{
			m_noiseLevel_keeper = m_noiseLevel;			//物音レベルを更新
			m_noise_Pos_keeper = m_noise_Pos;			//物音座標を更新
			m_hearingTime = 0;	//聴覚時間リセット
		}
		if (m_noiseLevel_keeper > 5 && m_noiseLevel_keeper != 0)
		{
			m_noiseLevel_keeper = m_noiseLevel;			//物音レベルを更新
			m_noise_Pos_keeper = m_noise_Pos;			//物音座標を更新
			m_hearingTime = 0;	//聴覚時間リセット
		}

	}
	else
	{
		m_noiseLevel_keeper = m_noiseLevel;			//物音レベルを更新
		m_noise_Pos_keeper = m_noise_Pos;			//物音座標を更新
		m_hearingTime = 0;	//聴覚時間リセット
	}
}

//------------------------------------------------------------------------------------------------------------
//探索処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateSearch(float _deltaTime)
{
	//プレイヤーキャラが見つからなければ終了
	if (!m_pPlayerChara) { return; }

	//聴覚と視界のどちらを優先するか
	if (m_noiseCheck && m_visionCheck)
	{
		if (m_visionLevel >= m_noiseLevel_keeper)
		{
			m_noiseCheck = false;		//聴覚OF
		}
		else
		{
			m_visionCheck = false;		//視界OF

		}
	}
	//ステータス変更
	//視界によるステータスの変更
	//m_visionLevel 	   =Patrol(0,1) Doubt(2) Caution(3) Battle(4,5) Miss(6,7) Return(8,9)
	//m_noiseLevel_keeper  =Patrol(0,1) Doubt(2) Caution(3) Battle(4,5) Miss(6,7) Return(8,9)
	if (m_battleCheck)
	{
		m_enemyCurrentState = EEnemyStatus::Battle;

	}
	else if (m_cautionCheck)
	{
		m_enemyCurrentState = EEnemyStatus::Caution;

	}
	else if (m_visionCheck)
	{
		//巡回
		if (m_visionLevel <= 1)
		{
			m_enemyCurrentState = EEnemyStatus::Patrol;
		}
		//疑念
		else if (m_visionLevel == 2)
		{
			m_enemyCurrentState = EEnemyStatus::Doubt;
		}
		//注意
		else if (m_visionLevel == 3)
		{
			m_enemyCurrentState = EEnemyStatus::Caution;
		}
		//戦闘
		else if (m_visionLevel == 4 || m_visionLevel == 5)
		{
			m_enemyCurrentState = EEnemyStatus::Battle;
		}
	}
	else if (m_noiseCheck)
	{
		if (m_noiseLevel_keeper <= 1)
		{
			m_enemyCurrentState = EEnemyStatus::Patrol;
		}
		else if (m_noiseLevel_keeper == 2)
		{
			m_enemyCurrentState = EEnemyStatus::Doubt_Noise;
		}
		else if (m_noiseLevel_keeper == 3)
		{
			m_enemyCurrentState = EEnemyStatus::Caution_Noise;
		}
		else if (m_noiseLevel_keeper == 4 || m_noiseLevel_keeper == 5)
		{
			m_enemyCurrentState = EEnemyStatus::Battle_Noise;
		}
	}
	//失踪の場合
	else if (m_visionLevel == 6 || m_visionLevel == 7 || m_noiseLevel_keeper == 6 || m_noiseLevel_keeper == 7)
	{
		m_enemyCurrentState = EEnemyStatus::Miss;
	}
	//帰還の場合
	else if (m_visionLevel == 8 || m_visionLevel == 9 || m_noiseLevel_keeper == 8 || m_noiseLevel_keeper == 9)
	{
		m_enemyCurrentState = EEnemyStatus::Return;
	}
	//巡回の場合
	else if (m_visionLevel <= 1 || m_noiseLevel_keeper <= 1)
	{
		m_enemyCurrentState = EEnemyStatus::Patrol;
	}


}

//------------------------------------------------------------------------------------------------------------
//警戒の処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateAlert(float _deltaTime)
{
	//アラートチェック
	if (!m_alertCheck)
	{
		return;
	}

	//警戒時間の持続
	if (m_enemyCurrentState == EEnemyStatus::Battle || m_enemyCurrentState == EEnemyStatus::Battle_Noise)
	{
		m_alertTime = 0;
		return;
	}

	m_alertTime += _deltaTime;		//警戒時間の経過

	//警戒時間が一定時間経過した場合
	if (m_alertTime >= m_alertTime_Limit)
	{
		m_alertCheck = false;
	}
}

//------------------------------------------------------------------------------------------------------------
//現在のステータスを参照しリセットする処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::ResetStateValues(float _deltaTime)
{


	//一つ前のステータスを保存
	if (m_enemyCurrentState != m_enemyCurrentState_Check && m_enemyCurrentState_Check != m_enemyCurrentState_Keeper)
	{
		m_enemyCurrentState_Keeper = m_enemyCurrentState_Check;
	}
	if (m_enemyCurrentState != m_enemyCurrentState_Check && m_enemyCurrentState_Check == m_enemyCurrentState_Keeper)
	{
		m_enemyCurrentState_Check = m_enemyCurrentState;
	}

	//m_visionLevel 	   =Patrol(0,1) Doubt(2) Caution(3) Battle(4,5) Miss(6,7) Return(8,9)
	//m_noiseLevel_keeper  =Patrol(0,1) Doubt(2) Caution(3) Battle(4,5) Miss(6,7) Return(8,9)

	//以前のステータスが巡回の場合
	if (m_enemyCurrentState_Keeper == EEnemyStatus::Patrol)
	{
		m_patrol_TurningCheck = false;		//旋回終了

		m_patrol_TurningCheckingTime = 0;	//旋回タイマーリセット

		m_patrolCheck = false;				//巡回チェックOF
	}

	//以前のステータスが疑念の場合
	else if (m_enemyCurrentState_Keeper == EEnemyStatus::Doubt)
	{
		m_doubtCheck = false;	//疑念チェックOF
		m_doubtTime = 0;		//疑念タイマーリセット
	}

	//以前のステータスが物音疑念の場合
	else if (m_enemyCurrentState_Keeper == EEnemyStatus::Doubt_Noise)
	{
		m_doubtNoiseCheck = false;	//物音疑念チェックOF
		m_doubtNoiseTime = 0;		//物音疑念タイマーリセット
	}

	//以前のステータスが注意の場合
	else if (m_enemyCurrentState_Keeper == EEnemyStatus::Caution)
	{
		m_cautionCheck = false;	//注意チェックOF
		m_cautionTime = 0;		//注意タイマーリセット
	}

	//以前のステータスが物音注意の場合
	else if (m_enemyCurrentState_Keeper == EEnemyStatus::Caution_Noise)
	{
		m_cautionNoiseCheck = false;//物音注意チェックOF
		m_cautionNoiseTime = 0;		//物音注意タイマーリセット
	}

	//以前のステータスが戦闘の場合
	else if (m_enemyCurrentState_Keeper == EEnemyStatus::Battle)
	{
		m_battleCheck = false;	//戦闘チェックOF
		m_battleTime = 0;		//戦闘タイマーリセット
	}

	//以前のステータスが物音戦闘の場合
	else if (m_enemyCurrentState_Keeper == EEnemyStatus::Battle_Noise)
	{
		m_battleNoiseCheck = false;	//物音戦闘チェックOF
		m_battleNoiseTime = 0;		//物音戦闘タイマーリセット
	}

	//以前のステータスが失踪の場合
	else if (m_enemyCurrentState_Keeper == EEnemyStatus::Miss)
	{
		m_missCheck = false;	//失踪チェックOF
		m_missTime = 0;			//失踪時間リセット
	}

	//以前のステータスが帰還の場合
	else if (m_enemyCurrentState_Keeper == EEnemyStatus::Return)
	{
		//もし動いていたらMoveToを止める
		if (m_returnCheck)
		{
			m_moveStop_Nav = true;			//Nav移動終了
			UpdateMove_Nav(_deltaTime);		//
		}
		m_returnCheck = false;		//帰還チェックOF
		m_returnTime = 0;			//帰還時間リセット
	}

	if (!m_noiseCheck)
	{
		m_noise_Pos = FVector(-5000., -5000., -5000.);		//物音座標リセット
		m_noise_Pos_keeper = FVector(-5000., -5000., -5000.);//物音座標リセット
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータス管理の処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateStatus(float _deltaTime)
{
	if (m_enemyInfo.hp <= 0)
	{
		CaseDead(_deltaTime);
		return;
	}

	//エネミーのステータス
	switch (m_enemyCurrentState)
	{
	case EEnemyStatus::Patrol:
		CasePatrol(_deltaTime);
		break;

	case EEnemyStatus::Doubt:
		CaseDoubt(_deltaTime);
		break;

	case EEnemyStatus::Doubt_Noise:
		CaseDoubt_Noise(_deltaTime);
		break;

	case EEnemyStatus::Caution:
		CaseCaution(_deltaTime);
		break;

	case EEnemyStatus::Caution_Noise:
		CaseCaution_Noise(_deltaTime);
		break;

	case EEnemyStatus::Battle:
		CaseBattle(_deltaTime);
		break;

	case EEnemyStatus::Battle_Noise:
		CaseBattle_Noise(_deltaTime);
		break;

	case EEnemyStatus::Miss:
		CaseMiss(_deltaTime);
		break;

	case EEnemyStatus::Return:
		CaseReturn(_deltaTime);
		break;

	case EEnemyStatus::Dead:
		CaseDead(_deltaTime);
		break;
	}

}

//------------------------------------------------------------------------------------------------------------
//ステータスが巡回の場合の処理(0,1)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CasePatrol(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_patrolCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Patrol"));

		m_patrolCheck = true;
		m_currentChaseSpeed = m_chaseSpeed_Slow;		//追跡速度の変更
		GetCharacterMovement()->MaxWalkSpeed = m_currentChaseSpeed;
	}

	m_enemyPos_Return = GetActorLocation();		//帰還の位置を保持

	m_enemyDirection_Return = GetActorLocation() + (GetActorForwardVector() * 100);//帰還の方向を保持

	//移動中止
	if (m_patrolCancel)
	{
		//	UE_LOG(LogTemp, Warning, TEXT("m_moveStop"));
		return;
	}

	FVector EnemyBaseForward_Pos = m_enemyPos + m_enemyForward * m_stopDistance_Wall;		//エネミーの正面,m_stopDistance_Wall先の座標

	//m_pEnemy_Routeになにも入っていない場合
	if (m_pEnemy_Route.Num() == 0)
	{
		//
		UE_LOG(LogTemp, Warning, TEXT("Patrol_NULL"));

		FHitResult HitCollision;	//ヒットした（＝コリジョン判定を受けた）オブジェクトを格納する変数

		//旋回中は呼ばない
		if (!m_patrol_TurningCheck)
		{
			//レイを飛ばし、全てのオブジェクトに対してコリジョン判定を行う
			bool isHit = GetWorld()->LineTraceSingleByObjectType(HitCollision, m_enemyPos, EnemyBaseForward_Pos, FCollisionObjectQueryParams::AllObjects, DefaultCollisionParams);

			//ヒットするオブジェクトがある場合
			if (isHit)
			{
				AActor* hitActor = HitCollision.GetActor();

				//壁,エネミーに当たらなかった場合
				if (!hitActor->ActorHasTag("Wall") && !hitActor->ActorHasTag("Enemy"))
				{
					m_routePos = EnemyBaseForward_Pos;

					UpdateMove(_deltaTime);
				}
			}
			else
			{
				m_routePos = EnemyBaseForward_Pos;

				UpdateMove(_deltaTime);
				return;
			}
		}

		m_enemyPos_XY_Wall[0] = m_enemyPos + FVector(0.f, 1.0f, 0.f) * m_stopDistance_Wall;		//エネミー座標からY+座標(0の場合)
		m_enemyPos_XY_Wall[1] = m_enemyPos + FVector(1.0f, 0.f, 0.f) * m_stopDistance_Wall;		//エネミー座標からX+座標(1の場合)
		m_enemyPos_XY_Wall[2] = m_enemyPos + FVector(0.f, -1.0f, 0.f) * m_stopDistance_Wall;		//エネミー座標からY-座標(2の場合)
		m_enemyPos_XY_Wall[3] = m_enemyPos + FVector(-1.0f, 0.f, 0.f) * m_stopDistance_Wall;		//エネミー座標からX-座標(3の場合)

		if (!m_patrol_TurningCheck)
		{
			int Num[4] = { 0,0,0,0 };		//壁のない方向識別用
			int Num_1 = 0;					//壁のない方向の数

			for (int i = 0; i < 4; i++)
			{
				//レイを飛ばし、全てのオブジェクトに対してコリジョン判定を行う
				bool isHitXY = GetWorld()->LineTraceSingleByObjectType(HitCollision, m_enemyPos, m_enemyPos_XY_Wall[i], FCollisionObjectQueryParams::AllObjects, DefaultCollisionParams);

				//ヒットするオブジェクトがある場合
				if (isHitXY)
				{
					AActor* hitActor = HitCollision.GetActor();

					//壁、エネミーに当たらなかった場合
					if (!hitActor->ActorHasTag("Wall") && !hitActor->ActorHasTag("Enemy"))
					{
						Num[Num_1] = i;				//壁のない方向を追加
						Num_1 += 1;					//壁のない方向の数を追加
					}
				}
				else
				{
					Num[Num_1] = i;				//壁のない方向を追加
					Num_1 += 1;					//壁のない方向の数を追加
				}
			}

			//進行方向用ランダム関数
			int Random = FMath::RandRange(0, Num_1 - 1);
			int RandomNum = Random;

			//壁のない方向をランダムで選択
			for (int i = 0; i < Num_1; ++i)
			{
				if (RandomNum == i)
				{
					m_patrol_TurnDirection = Num[i];				//決定した方向の格納
				}
			}
			m_patrol_TurningCheck = true;		//旋回中
		}

		//移動方向決定
		FRotator DirectionRot = UKismetMathLibrary::FindLookAtRotation(m_enemyPos, m_enemyPos_XY_Wall[m_patrol_TurnDirection]);
		FRotator newRot = FMath::RInterpTo(GetActorRotation(), DirectionRot, _deltaTime, m_chaseRotSpeed);
		//旋回
		SetActorRotation(FRotator(GetActorRotation().Pitch, newRot.Yaw, GetActorRotation().Roll));

		m_patrol_TurningCheckingTime += _deltaTime;	//旋回経過時間

		//旋回時間が経過した場合
		if (m_patrol_TurningCheckingTime >= m_patrol_TurningCheckingTime_Limit)
		{
			m_patrol_TurningCheck = false;			//旋回終了
			m_patrol_TurningCheckingTime = 0;		//旋回タイマーリセット
		}
	}

	//ランダムルートの場合
	else if (m_randomRoute)
	{
		FHitResult HitCollision;	//ヒットした（＝コリジョン判定を受けた）オブジェクトを格納する変数

		//旋回中は呼ばない
		if (!m_patrol_TurningCheck)
		{
			//レイを飛ばし、全てのオブジェクトに対してコリジョン判定を行う
			bool isHit = GetWorld()->LineTraceSingleByObjectType(HitCollision, m_enemyPos, EnemyBaseForward_Pos, FCollisionObjectQueryParams::AllObjects, DefaultCollisionParams);

			//ヒットするオブジェクトがある場合
			if (isHit)
			{
				AActor* hitActor = HitCollision.GetActor();

				//壁,エネミーに当たらなかった場合
				if (!hitActor->ActorHasTag("Wall") && !hitActor->ActorHasTag("Enemy"))
				{
					m_routePos = EnemyBaseForward_Pos;

					UpdateMove(_deltaTime);
				}
			}
			else
			{
				m_routePos = EnemyBaseForward_Pos;

				UpdateMove(_deltaTime);
				return;
			}
		}

		m_enemyPos_XY_Wall[0] = m_enemyPos + FVector(0.f, 1.0f, 0.f) * m_stopDistance_Wall;		//エネミー座標からY+座標(0の場合)
		m_enemyPos_XY_Wall[1] = m_enemyPos + FVector(1.0f, 0.f, 0.f) * m_stopDistance_Wall;		//エネミー座標からX+座標(1の場合)
		m_enemyPos_XY_Wall[2] = m_enemyPos + FVector(0.f, -1.0f, 0.f) * m_stopDistance_Wall;		//エネミー座標からY-座標(2の場合)
		m_enemyPos_XY_Wall[3] = m_enemyPos + FVector(-1.0f, 0.f, 0.f) * m_stopDistance_Wall;		//エネミー座標からX-座標(3の場合)

		if (!m_patrol_TurningCheck)
		{
			int Num[4] = { 0,0,0,0 };		//壁のない方向識別用
			int Num_1 = 0;					//壁のない方向の数

			for (int i = 0; i < 4; i++)
			{
				//レイを飛ばし、全てのオブジェクトに対してコリジョン判定を行う
				bool isHitXY = GetWorld()->LineTraceSingleByObjectType(HitCollision, m_enemyPos, m_enemyPos_XY_Wall[i], FCollisionObjectQueryParams::AllObjects, DefaultCollisionParams);

				//ヒットするオブジェクトがある場合
				if (isHitXY)
				{
					AActor* hitActor = HitCollision.GetActor();

					//壁、エネミーに当たらなかった場合
					if (!hitActor->ActorHasTag("Wall") && !hitActor->ActorHasTag("Enemy"))
					{
						Num[Num_1] = i;				//壁のない方向を追加
						Num_1 += 1;					//壁のない方向の数を追加
					}
				}
				else
				{
					Num[Num_1] = i;				//壁のない方向を追加
					Num_1 += 1;					//壁のない方向の数を追加
				}
			}

			//進行方向用ランダム関数
			int Random = FMath::RandRange(0, Num_1 - 1);
			int RandomNum = Random;

			//壁のない方向をランダムで選択
			for (int i = 0; i < Num_1; ++i)
			{
				if (RandomNum == i)
				{
					m_patrol_TurnDirection = Num[i];				//決定した方向の格納
				}
			}
			m_patrol_TurningCheck = true;		//旋回中
		}

		//移動方向決定
		FRotator DirectionRot = UKismetMathLibrary::FindLookAtRotation(m_enemyPos, m_enemyPos_XY_Wall[m_patrol_TurnDirection]);
		FRotator newRot = FMath::RInterpTo(GetActorRotation(), DirectionRot, _deltaTime, m_chaseRotSpeed);
		//旋回
		SetActorRotation(FRotator(GetActorRotation().Pitch, newRot.Yaw, GetActorRotation().Roll));

		m_patrol_TurningCheckingTime += _deltaTime;	//旋回経過時間

		//旋回時間が経過した場合
		if (m_patrol_TurningCheckingTime >= m_patrol_TurningCheckingTime_Limit)
		{
			m_patrol_TurningCheck = false;			//旋回終了
			m_patrol_TurningCheckingTime = 0;		//旋回タイマーリセット
		}
	}

	//決められたルートを歩く
	else if (!m_randomRoute)
	{
		//全ての地点を回ったら初めの地点にリセット
		if (m_routeCounter >= m_pEnemy_Route.Num())
		{
			m_routeCounter = 0;
		}

		m_patrolTime += _deltaTime;

		//行先の設定
		m_routePos = m_pEnemy_Route[m_routeCounter]->GetActorLocation();

		FHitResult HitCollision;	//ヒットした（＝コリジョン判定を受けた）オブジェクトを格納する変数

		//レイを飛ばし、全てのオブジェクトに対してコリジョン判定を行う
		bool isHit = GetWorld()->LineTraceSingleByObjectType(HitCollision, m_enemyPos, EnemyBaseForward_Pos, FCollisionObjectQueryParams::AllObjects, DefaultCollisionParams);

		//ヒットするオブジェクトがある場合
		if (isHit)
		{
			AActor* hitActor = HitCollision.GetActor();

			//エネミーに当たった場合
			if (hitActor->ActorHasTag("Enemy"))
			{
				m_patrol_TurningCheck = true;
				m_turnRight_Pos = GetActorLocation() + m_enemyForward * EnemyBaseForward_Pos + m_enemyRight * EnemyBaseForward_Pos;
			}
		}

		double distance_2D = FVector::Dist2D(m_routePos, m_enemyPos);	//ルート地点とエネミーのとの距離を測る(Vectorの長さ）

		//少し右にずれる
		if (m_patrol_TurningCheck)
		{
			m_patrol_TurningCheckingTime += _deltaTime;

			m_routePos = m_turnRight_Pos;

			UpdateViewMove(_deltaTime);
			UpdateMove(_deltaTime);
		}
		else
		{
			UpdateViewMove(_deltaTime);

			if (m_stopDistance_2D < distance_2D && m_patrolTime > m_patrolTime_Limit)
			{
				UpdateMove(_deltaTime);
			}

		}

		//一定時間たつと元の動きに戻る
		if (m_patrol_TurningCheckingTime > m_patrol_TurningTime_Limit)
		{
			m_patrol_TurningCheck = false;
			m_patrol_TurningCheckingTime = 0;
		}

		//地点についたら次の地点へ
		if (m_stopDistance_2D >= distance_2D)
		{
			m_patrolTime = 0;
			m_routeCounter += 1;
		}
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが疑念の場合の処理(2)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseDoubt(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_doubtCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Doubt"));

		m_doubtCheck = true;
		m_currentChaseSpeed = m_chaseSpeed_Normal;		//追跡速度の変更
		GetCharacterMovement()->MaxWalkSpeed = m_currentChaseSpeed;
	}

	m_doubtTime += _deltaTime;				//疑念時間の経過

	//視点移動処理
	UpdateViewMove(_deltaTime);

	//時間が経過すると注意状態に移行
	if (m_doubtTime > m_doubtTime_Limit)
	{
		//移動処理
		UpdateMove_Nav(_deltaTime);
	}


	//もし視界外に出た場合
	if (m_visionCheck == false)
	{
		m_visionLevel = 0;		//巡回に移行
		m_doubtTime = 0;			//疑念時間リセット
		m_visionCheck = false;
		m_doubtCheck = false;
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが物音疑念の場合の処理(2)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseDoubt_Noise(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_doubtNoiseCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Doubt_Noise"));

		m_doubtNoiseCheck = true;
	}

	m_doubtNoiseTime += _deltaTime;				//疑念時間の経過

	//視点移動処理
	UpdateViewMove(_deltaTime);

	//時間が経過すると終了(聴覚の場合)
	if (m_doubtNoiseTime > m_doubtNoiseTime_Limit)
	{
		m_noiseLevel_keeper = 0;		//巡回に移行
		m_doubtNoiseTime = 0;			//疑念時間リセット
		m_noiseCheck = false;		//聴覚中止
		m_doubtNoiseCheck = false;
		m_noise_Pos = FVector(-5000.0, -5000., -5000.);//物音座標リセット
		m_noise_Pos_keeper = FVector(-5000.0, -5000., -5000.);//物音座標リセット
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが注意の場合の処理(3)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseCaution(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_cautionCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Caution"));

		m_cautionCheck = true;
		m_currentChaseSpeed = m_chaseSpeed_Normal;		//追跡速度の変更
		GetCharacterMovement()->MaxWalkSpeed = m_currentChaseSpeed;

	}

	FVector2D playerPos_LastSeen(m_playerPos_LastSeen.X, m_playerPos_LastSeen.Y);
	FVector2D enemyPos(m_enemyPos.X, m_enemyPos.Y);

	double distance_2D = (playerPos_LastSeen - enemyPos).Length();	//物音との距離を測る(Vectorの長さ）

	//視点移動処理
	UpdateViewMove(_deltaTime);

	if (m_cautionTime < m_cautionTime_Limit)
	{
		//ひとつ前のステータスが疑念だった場合
		if (m_enemyCurrentState_Keeper == EEnemyStatus::Doubt)
		{
			m_cautionTime = m_cautionTime_Limit;				//発見猶予時間カット
		}
		else
		{
			m_cautionTime += _deltaTime;				//発見猶予時間
		}
	}
	if (m_cautionTime >= m_cautionTime_Limit)
	{
		//移動処理
		UpdateMove_Nav(_deltaTime);
	}

	//もしプレイヤーがいた位置に近づいた場合
	if (m_visionCheck == false && m_stopDistance_2D >= distance_2D)
	{
		m_moveStop_Nav = true;		//停止（Nav）
		UpdateMove_Nav(_deltaTime);
		m_visionLevel = 6;			//見失うに移行
		m_cautionCheck = false;		//注意終了
		m_visionCheck = false;
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが物音注意の場合の処理(3)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseCaution_Noise(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_cautionNoiseCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Caution_Noise"));

		m_cautionNoiseCheck = true;
		m_currentChaseSpeed = m_chaseSpeed_Normal;		//追跡速度の変更
		GetCharacterMovement()->MaxWalkSpeed = m_currentChaseSpeed;

	}

	FVector2D noise_Pos_keeper(m_noise_Pos_keeper.X, m_noise_Pos_keeper.Y);
	FVector2D enemyPos(m_enemyPos.X, m_enemyPos.Y);

	double distance_2D = (noise_Pos_keeper - enemyPos).Length();	//物音との距離を測る(Vectorの長さ）

	m_cautionNoiseTime += _deltaTime;

	//視点移動処理
	UpdateViewMove(_deltaTime);

	//時間が経過すると移動
	if (m_cautionNoiseTime > m_cautionNoiseTime_Limit)
	{
		//移動処理
		UpdateMove_Nav(_deltaTime);
	}

	//物音に近づいた場合
	if (distance_2D < m_stopDistance_Noise)
	{
		m_moveStop_Nav = true;		//停止（Nav）
		UpdateMove_Nav(_deltaTime);
		m_noiseLevel_keeper = 6;		//見失うに移行
		m_noiseCheck = false;		//聴覚中止
		m_cautionNoiseCheck = false;
		m_noise_Pos = FVector(-5000.0, -5000., -5000.);//物音座標リセット
		m_noise_Pos_keeper = FVector(-5000.0, -5000., -5000.);//物音座標リセット
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが戦闘の場合の処理(4,5)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseBattle(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_battleCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Battle"));

		m_battleCheck = true;//戦闘中
		m_alertCheck = true;	//警戒状態ON
		m_currentChaseSpeed = m_chaseSpeed_Fast;							//追跡速度の変更
		GetCharacterMovement()->MaxWalkSpeed = m_currentChaseSpeed;
		m_battleFalseTime = 0;

	}

	//視点移動処理
	UpdateViewMove(_deltaTime);

	FVector2D playerPos_LastSeen(m_playerPos_LastSeen.X, m_playerPos_LastSeen.Y);
	FVector2D enemyPos(m_enemyPos.X, m_enemyPos.Y);

	double distance_2D = (playerPos_LastSeen - enemyPos).Length();	//最後に見たプレイヤーとの距離を測る(Vectorの長さ）

	double distance = (m_playerPos_LastSeen - m_enemyPos).Length();	//最後に見たプレイヤーとの距離を測る(Vectorの長さ）


	if (m_battleTime < m_battleTime_Limit)
	{
		//ひとつ前のステータスが巡回もしくは帰還以外だった場合
		if (m_enemyCurrentState_Keeper != EEnemyStatus::Patrol && m_enemyCurrentState_Keeper != EEnemyStatus::Return)
		{
			m_battleTime = m_battleTime_Limit;				//発見猶予時間カット
		}
		else
		{
			m_battleTime += _deltaTime;				//発見猶予時間
		}
	}

	//発見猶予時間
	if (m_battleTime >= m_battleTime_Limit)
	{
		//プレイヤーの一定距離に近づくまで追いかける
		if (m_stopDistance_Player < distance)
		{
			//移動処理
			UpdateMove_Nav(_deltaTime);
		}

		if (m_sword)
		{
			UE_LOG(LogTemp, Warning, TEXT("Swing"));
			m_sword->Swinging(false);
		}

		//武器を持っていたら
		if (m_pEnemy_Weapon)
		{

			//攻撃範囲に入ったら攻撃
			if (m_attackDistance > distance)
			{

				FVector WeaponPos = m_pEnemy_Weapon->GetActorLocation();
				FVector WeaponForwardVector = m_pEnemy_Weapon->GetActorForwardVector();
				FVector WeaponForwardPos = WeaponPos + WeaponForwardVector * m_attackDistance;

				FHitResult HitCollision;		//ヒットした（＝コリジョン判定を受けた）オブジェクトを格納する変数

				//レイを飛ばし、全てのオブジェクトに対してコリジョン判定を行う
				bool isHit = GetWorld()->LineTraceSingleByObjectType(HitCollision, WeaponPos, WeaponForwardPos, FCollisionObjectQueryParams::AllObjects, BattleCollisionParams);

				//ヒットするオブジェクトがある場合
				if (isHit)
				{
					AActor* hitActor = HitCollision.GetActor();

					//当たったコリジョンがプレイヤーだった場合
					if (hitActor->ActorHasTag("Player"))
					{
						//攻撃処理
						UpdateAttack(_deltaTime);
					}
				}
			}
		}
	}

	//見失った後どっちの方向に曲がったか覚えておく
	if (!m_visionCheck)
	{
		if (m_battleFalseTime <= m_battleFalseTime_Limit)
		{
			m_battleFalseTime += _deltaTime;
			m_playerPos_Nav_LastSeen = m_playerPos;
		}
	}

	//もしプレイヤーがいた位置に近づいた場合
	if (m_visionCheck == false && m_stopDistance_Player >= distance_2D)
	{
		m_moveStop_Nav = true;		//停止（Nav）
		UpdateMove_Nav(_deltaTime);
		m_visionLevel = 6;			//見失うに移行
		m_battleCheck = false;		//戦闘終了
		m_visionCheck = false;
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが物音戦闘の場合の処理(4,5)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseBattle_Noise(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_battleNoiseCheck)
	{
		UE_LOG(LogTemp, Warning, TEXT("Battle_Noise"));

		m_battleNoiseCheck = true;//戦闘中
		m_alertCheck = true;	//警戒状態ON
		m_currentChaseSpeed = m_chaseSpeed_Fast;							//追跡速度の変更
		GetCharacterMovement()->MaxWalkSpeed = m_currentChaseSpeed;
	}

	FVector2D noise_Pos_keeper(m_noise_Pos_keeper.X, m_noise_Pos_keeper.Y);
	FVector2D enemyPos(m_enemyPos.X, m_enemyPos.Y);

	double distance_2D = (noise_Pos_keeper - enemyPos).Length();	//物音との距離を測る(Vectorの長さ）
	m_currentChaseSpeed = m_chaseSpeed_Fast;							//追跡速度の変更

	if (m_battleNoiseTime < m_battleNoiseTime_Limit)
	{
		//ひとつ前のステータスが失踪だった場合
		if (m_enemyCurrentState_Keeper == EEnemyStatus::Miss)
		{
			m_battleNoiseTime = m_battleTime_Limit;				//発見猶予時間カット
		}
		else
		{
			m_battleNoiseTime += _deltaTime;				//発見猶予時間
		}
	}

	//視点移動処理
	UpdateViewMove(_deltaTime);

	//発見猶予時間
	if (m_battleNoiseTime >= m_battleNoiseTime_Limit)
	{
		//移動処理
		UpdateMove_Nav(_deltaTime);
	}

	if (m_stopDistance_Noise > distance_2D)
	{
		UE_LOG(LogTemp, Warning, TEXT("Battle_NoiseStop"));

		m_moveStop_Nav = true;		//停止（Nav）
		UpdateMove_Nav(_deltaTime);
		m_noiseLevel_keeper = 6;			//失踪に移行
		m_battleNoiseCheck = false;//戦闘中
		m_noiseCheck = false;		//聴覚中止
		m_noise_Pos = FVector(-5000.0, -5000., -5000.);//物音座標リセット
		m_noise_Pos_keeper = FVector(-5000.0, -5000., -5000.);//物音座標リセット
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが失踪の場合の処理(6,7)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseMiss(float _deltaTime)
{
	//呼び始め1回のみ
	if (!m_missCheck)
	{
		//
		UE_LOG(LogTemp, Warning, TEXT("Miss"));
		m_missCheck = true;//失踪中
		m_visionCheck = false;
		m_noiseCheck = false;

	}

	m_missTime += _deltaTime;						//失踪している時間

	//リミット時間の設定
	const float m_missTime_Limit1 = m_missTime_Limit;
	const float m_missTime_Limit2 = m_missTime_Limit * 2;
	const float m_missTime_Limit3 = m_missTime_Limit * 3;
	const float m_missTime_Limit4 = m_missTime_Limit * 4;

	//前のステータスが戦闘もしくは注意だった場合
	if (m_missTime < m_missTime_Limit1 && m_enemyCurrentState_Keeper == EEnemyStatus::Battle)
	{
		m_enemyPos_Forward_Miss = m_enemyPos + m_enemyForward * m_visionRange_Short;			//エネミーの正面の座標
		m_enemyPos_Left_Miss = m_enemyPos - GetActorRightVector() * m_visionRange_Short;		//エネミー座標から左座標
		m_enemyPos_Right_Miss = m_enemyPos + GetActorRightVector() * m_visionRange_Short;		//エネミー座標から右座標

		//移動処理
		UpdateMove_Nav(_deltaTime);
		return;
	}

	//少し正面に進む
	else if (m_missTime < m_missTime_Limit1)
	{
		m_enemyPos_Forward_Miss = m_enemyPos + m_enemyForward * m_visionRange_Short;			//エネミーの正面の座標
		m_enemyPos_Left_Miss = m_enemyPos - GetActorRightVector() * m_visionRange_Short;		//エネミー座標から左座標
		m_enemyPos_Right_Miss = m_enemyPos + GetActorRightVector() * m_visionRange_Short;		//エネミー座標から右座標

		UpdateMove(_deltaTime);

		FVector EnemyBaseForward_Pos = m_enemyPos + m_enemyForward * m_stopDistance_Wall;		//エネミーの正面,m_stopDistance_Wall先の座標

		FHitResult HitCollision;	//ヒットした（＝コリジョン判定を受けた）オブジェクトを格納する変数

		//レイを飛ばし、全てのオブジェクトに対してコリジョン判定を行う
		bool isHit = GetWorld()->LineTraceSingleByObjectType(HitCollision, m_enemyPos, EnemyBaseForward_Pos, FCollisionObjectQueryParams::AllObjects, DefaultCollisionParams);

		//ヒットするオブジェクトがある場合
		if (isHit)
		{
			AActor* hitActor = HitCollision.GetActor();

			//当たったコリジョンが壁,エネミーだった場合
			if (hitActor->ActorHasTag("Wall") || hitActor->ActorHasTag("Enemy"))
			{
				m_missTime = 1;
			}
		}
		return;
	}
	//左方向の確認
	else if (m_missTime < m_missTime_Limit2)
	{
		//Navをストップ
		m_moveStop_Nav = true;
		UpdateMove_Nav(_deltaTime);

		//旋回方向の決定
		m_enemyPos_Direction_Miss = m_enemyPos_Left_Miss;
		//旋回
		UpdateViewMove(_deltaTime);
		return;
	}

	//右方向の確認
	else if (m_missTime < m_missTime_Limit3)
	{
		//旋回方向の決定
		m_enemyPos_Direction_Miss = m_enemyPos_Right_Miss;
		//旋回
		UpdateViewMove(_deltaTime);
		return;
	}

	//正面方向の確認
	else if (m_missTime < m_missTime_Limit4)
	{
		//旋回方向の決定
		m_enemyPos_Direction_Miss = m_enemyPos_Forward_Miss;
		//旋回
		UpdateViewMove(_deltaTime);
		return;
	}
	//失踪終了
	else
	{
		m_visionLevel = 8;				//帰還に移行
		m_noiseLevel_keeper = 8;		//帰還に移行
		m_missTime = 0;					//見失う時間リセット
		m_missCheck = false;	//見失い終了
	}
}

//------------------------------------------------------------------------------------------------------------
//ステータスが帰還の場合の処理(8,9)
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::CaseReturn(float _deltaTime)
{
	double distance = (m_enemyPos_Return - m_enemyPos).Length();	//プレイヤーとの距離を測る(Vectorの長さ）

	//呼び始め1回のみ
	if (!m_returnCheck)
	{
		//
		UE_LOG(LogTemp, Warning, TEXT("Return"));

		m_currentChaseSpeed = m_chaseSpeed_Normal;	//追跡速度の変更
		GetCharacterMovement()->MaxWalkSpeed = m_currentChaseSpeed;
		m_returnCheck = true;//戻り中
		m_visionCheck = false;
		m_noiseCheck = false;
	}

	if (m_stopDistance_Player < distance)
	{
		UpdateMove_Nav(_deltaTime);
	}

	if (m_stopDistance_Player >= distance)
	{
		m_moveStop_Nav = true;		//停止（Nav）
		UpdateMove_Nav(_deltaTime);
		m_returnTime += _deltaTime;
		UpdateViewMove(_deltaTime);
	}

	if (m_returnTime > m_returnTime_Limit)
	{
		m_visionLevel = 0;				//巡回に戻る
		m_noiseLevel_keeper = 0;				//巡回に戻る
		m_returnTime = 0;					//帰還時間リセット
		m_returnCheck = false;	//帰還終了
		//
		UE_LOG(LogTemp, Warning, TEXT("ReturnOK"));

	}
}

//------------------------------------------------------------------------------------------------------------
//視点移動の処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateViewMove(float _deltaTime)
{
	if (m_visionCheck || m_battleCheck || m_cautionCheck)
	{
		//旋回方向決定
		FRotator playerRot = UKismetMathLibrary::FindLookAtRotation(m_enemyPos, m_playerPos_LastSeen);
		FRotator newRot = FMath::RInterpTo(GetActorRotation(), playerRot, _deltaTime, m_chaseRotSpeed);
		//旋回
		SetActorRotation(FRotator(GetActorRotation().Pitch, newRot.Yaw, GetActorRotation().Roll));

	}
	else if (m_missCheck)
	{
		//旋回方向決定
		FRotator playerRot = UKismetMathLibrary::FindLookAtRotation(m_enemyPos, m_enemyPos_Direction_Miss);
		FRotator newRot = FMath::RInterpTo(GetActorRotation(), playerRot, _deltaTime, m_chaseRotSpeed);
		//旋回
		SetActorRotation(FRotator(GetActorRotation().Pitch, newRot.Yaw, GetActorRotation().Roll));
	}
	else if (m_returnCheck)
	{
		//旋回方向決定
		FRotator playerRot = UKismetMathLibrary::FindLookAtRotation(m_enemyPos, m_enemyDirection_Return);
		FRotator newRot = FMath::RInterpTo(GetActorRotation(), playerRot, _deltaTime, m_chaseRotSpeed);
		//旋回
		SetActorRotation(FRotator(GetActorRotation().Pitch, newRot.Yaw, GetActorRotation().Roll));
	}
	else if (m_patrolCheck)
	{
		//旋回方向決定
		FRotator playerRot = UKismetMathLibrary::FindLookAtRotation(m_enemyPos, m_routePos);
		FRotator newRot = FMath::RInterpTo(GetActorRotation(), playerRot, _deltaTime, m_chaseRotSpeed);
		//旋回
		SetActorRotation(FRotator(GetActorRotation().Pitch, newRot.Yaw, GetActorRotation().Roll));
	}
	else
	{
		//旋回方向決定
		FRotator noiseRot = UKismetMathLibrary::FindLookAtRotation(m_enemyPos, m_noise_Pos_keeper);
		FRotator newRot = FMath::RInterpTo(GetActorRotation(), noiseRot, _deltaTime, m_chaseRotSpeed);
		//旋回
		SetActorRotation(FRotator(GetActorRotation().Pitch, newRot.Yaw, GetActorRotation().Roll));

	}
}

//------------------------------------------------------------------------------------------------------------
//移動の処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateMove(float _deltaTime)
{
	if (m_visionCheck || m_battleCheck || m_cautionCheck)
	{
		//移動位置決定
		FVector TargetChase = (m_playerPos_LastSeen - m_enemyPos).GetSafeNormal();
		FVector newPos = m_enemyPos + TargetChase * m_currentChaseSpeed * _deltaTime;
		//プレイヤーに向かって移動
		SetActorLocation(FVector(newPos.X, newPos.Y, GetActorLocation().Z));
	}
	else if (m_missCheck)
	{
		//移動位置決定
		FVector TargetChase = (m_enemyPos_Forward_Miss - m_enemyPos).GetSafeNormal();
		FVector newPos = m_enemyPos + TargetChase * m_currentChaseSpeed * _deltaTime;
		//プレイヤーに向かって移動
		SetActorLocation(FVector(newPos.X, newPos.Y, GetActorLocation().Z));
	}

	else if (m_patrolCheck)
	{
		//移動位置決定
		FVector TargetChase = (m_routePos - m_enemyPos).GetSafeNormal();
		FVector newPos = m_enemyPos + TargetChase * m_currentChaseSpeed * _deltaTime;
		//プレイヤーに向かって移動
		SetActorLocation(FVector(newPos.X, newPos.Y, GetActorLocation().Z));
	}
	else
	{
		//移動位置決定
		FVector TargetChase = (m_noise_Pos_keeper - m_enemyPos).GetSafeNormal();
		FVector newPos = m_enemyPos + TargetChase * m_currentChaseSpeed * _deltaTime;
		//プレイヤーに向かって移動
		SetActorLocation(FVector(newPos.X, newPos.Y, GetActorLocation().Z));
	}

}

//------------------------------------------------------------------------------------------------------------
//移動(Nav)の処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateMove_Nav(float _deltaTime)
{
	AAIController* AIMove = Cast<AAIController>(GetController());

	if (!AIMove)
	{
		return;
	}

	//移動停止
	if (m_moveStop_Nav)
	{
		AIMove->StopMovement();
		m_moveStop_Nav = false;
		return;
	}

	//移動
	//視界の場合
	if (m_visionCheck || m_battleCheck || m_cautionCheck)
	{
		AIMove->MoveToLocation(m_playerPos_LastSeen, m_stopDistance_Nav, true, true, true, false);
	}
	//物音の場合
	else if (m_noiseCheck)
	{
		AIMove->MoveToLocation(m_noise_Pos_keeper, m_stopDistance_Nav, true, true, true, false);
	}
	//失踪の場合
	else if (m_missCheck)
	{
		AIMove->MoveToLocation(m_playerPos_Nav_LastSeen, m_stopDistance_Nav, true, true, true, false);
	}
	//帰還の場合
	else if (m_returnCheck)
	{
		AIMove->MoveToLocation(m_enemyPos_Return, m_stopDistance_Nav, true, true, true, false);
	}
}

//------------------------------------------------------------------------------------------------------------
//攻撃の処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::UpdateAttack(float _deltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
	//m_attackingTime += _deltaTime;

	//m_pEnemy_Weapon->BulletFire(m_allTime, this);

	m_sword->Swinging(false);
}

//------------------------------------------------------------------------------------------------------------
//攻撃が終わったことを通知する処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::OnAttackEnd()
{

}

//------------------------------------------------------------------------------------------------------------
//物音の受け取り
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::OnNoiseHeard(const int& _noiseVolume, const FVector& _pos)
{
	//
	if (_noiseVolume > 0)
	{
		m_noiseLevel = _noiseVolume + 1;
		m_noise_Pos = _pos;
	}
	//巡回状態にならない場合があるため
	else
	{
		m_noiseLevel = _noiseVolume;
		m_noise_Pos = _pos;
	}
	//警戒状態の場合
	if (m_alertCheck)
	{
		m_noiseLevel += 1;
	}
}

//------------------------------------------------------------------------------------------------------------
//衝突判定
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//オーバーラップしたのがプレイヤーの時のみ反応させたい
	//PlayerCharaBp->Actor->Tagに設定したタグ「Player」で、プレイヤー識別する
	if (OtherActor->ActorHasTag("Player"))
	{
		//プレイヤーにキャストしてアイテム獲得時のイベントを起こす
		APlayerCharacter* pPlayer = Cast<APlayerCharacter>(OtherActor);
		if (!pPlayer) { return; }		//NULLチェック

	}
}


//------------------------------------------------------------------------------------------------------------
//ダメージ処理
//------------------------------------------------------------------------------------------------------------
void AEnemyBase::OnDamage(const int& _damage, const FVector& _knockBackVector, const bool& _bSneakKill)
{
	//体力減少
	//デバック用に死亡
	m_deadCheck = false;
	m_enemyCurrentState = EEnemyStatus::Dead;

	if (_bSneakKill)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sneak Kill"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sneak Miss"));
	}
}