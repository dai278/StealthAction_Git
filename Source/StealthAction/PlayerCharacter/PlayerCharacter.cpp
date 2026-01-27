//----------------------------------------------------------
// 概要				：プレイヤーキャラを制御するCharacterオブジェクト
// 更新日　　　　 　：11/19　
//----------------------------------------------------------

#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "StealthAction/GameUtility/GameUtility.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h" 
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Noise/NoiseManager.h"
#include "Light/ExtendedSpotLightManager.h"
#include "EnemyManager/EnemyManager.h"
#include"Enemy/EnemyBase.h"
#include "Sword/SwordAttackComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/CollisionProfile.h"
#include "GameInstance/PlayDataInfo.h"
#include "GameInstance/PlayDataGameInstanceSubsystem.h"
#include "Checkpoint/CheckpointManager.h"
#include "Interact/Interact.h"




//----------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------
APlayerCharacter::APlayerCharacter()
	:m_pSpringArm(nullptr)
	, m_pCamera(nullptr)
	, m_pMPC(nullptr)
	, m_charaMoveInput(FVector2D::ZeroVector)
	, m_cameraRotateInput(FVector2D::ZeroVector)
	, m_cameraPitchLimit(FVector2D(-80.f, 80.f))
	, m_cameraStatus(ECameraStatus::ThirdPerson)
	, m_cameraRotateSpeed(3.f)
	, m_WalkSpeed(300.f)
	, m_DashSpeed(600.f)
	, m_CrouchSpeed(100.f)
	, m_JumpVector(1500.f)
	, m_bCameraSwitching(false)
	, m_status(EPlayerStatus::Idle)
	, m_bCanControl(true)
	, DefaultMappingContext(nullptr)
	, m_moveIA(nullptr)
	, m_moveJumpIA(nullptr)
	, m_moveDashIA(nullptr)
	, m_moveCrouchIA(nullptr)
	, m_changeWeaponIA(nullptr)
	, m_attackIA(nullptr)
	, m_cameraPitchIA(nullptr)
	, m_cameraYawIA(nullptr)
	, m_cameraResetIA(nullptr)
	, m_cameraSwitchIA(nullptr)
	, m_defaultMesh(nullptr)
	, m_isShadowMesh(nullptr)
	, m_bUsingMesh(false)
	, m_bOnShadow(false)
	, m_Capsule(nullptr)
	, m_capsuleRadius(0.f)
	, m_capsuleHeight(0.f)
	, m_bIsCrouch(false)
	, m_maxShadowTime(5.f)
	, m_shadowTimer(0.f)
	, m_attackRange(300.f)
	, m_attackRadius(70.f)
	, m_sneakKillDamage(10)
	, m_NormalAttack(2)
	, m_bCanAttack(true)
	, m_attackCooldown(0.3f)
	, m_pExtendedSpotLightManager(nullptr)
	, m_bInvincible(false)
	, m_invincibleTimer(0.f)
	, m_invincibleTimeLimit(3.0f)
	, m_knockBackVelocity(FVector::ZeroVector)
	, m_playerInfo()
	,m_damageTime(1.f)

{
	//毎フレームTickを呼ぶか決めるフラグ
	PrimaryActorTick.bCanEverTick = true;

	//カメラ回転にキャラを直接追従させない
	bUseControllerRotationYaw = false;

	//自動回転を行わない
	GetCharacterMovement()->bOrientRotationToMovement = false;


	//デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//動的配列TArrayの初期化
	m_pHitActors.Reset();

	//カメラの視点変更時初期位置
	m_cameraInitPos[(int)ECameraStatus::ThirdPerson] = { FRotator{ 0.f,0.f,0.f},90.f,500.f };
	m_cameraInitPos[(int)ECameraStatus::TopDownView] = { FRotator{ 0.f,0.f,0.f},90.f,1000.f };
	m_cameraInitPos[(int)ECameraStatus::InShadow] = { FRotator{ 0.f,0.f,0.f},60.f,250.f };
	m_cameraInitPos[(int)ECameraStatus::Crouch] = { FRotator{ 0.f,0.f,0.f},70.f,350.f };


	//カプセルの初期値を記録
	m_Capsule = GetCapsuleComponent();
	m_capsuleRadius = 40.f;
	m_capsuleHeight = 80.f;

	//スプリングアームのオブジェクトを生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL) {
		m_pSpringArm->SetupAttachment(RootComponent);

		m_pSpringArm->TargetArmLength = m_cameraInitPos[(int)ECameraStatus::ThirdPerson].springArmLength;					//カメラ距離
		m_pSpringArm->SocketOffset = FVector(0.f, 50.f, 60.f);	//肩越し視点設定
		m_pSpringArm->bUsePawnControlRotation = true;			//カメラ回転
		m_pSpringArm->CameraLagSpeed = 8.f;
		m_pSpringArm->bDoCollisionTest = true;                 // 壁自動回避
	}


	//剣の攻撃コンポーネント生成
	m_sword = CreateDefaultSubobject<USwordAttackComponent>(TEXT("Sword"));
	if (m_sword)
	{
		m_sword->SetupAttachment(GetCapsuleComponent());
		m_sword->SetRelativeLocation(FVector::ZeroVector);
		m_sword->SetRelativeRotation(FRotator::ZeroRotator);

	}

	//カメラオブジェクトを設定
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if ((m_pCamera != nullptr) && (m_pSpringArm != nullptr)) {
		//カメラをスプリングアームにタッチさせる
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
		m_pCamera->bUsePawnControlRotation = false;
	}

}


//----------------------------------------------------------
// BeginPlay（ゲーム開始時）
//----------------------------------------------------------
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->bOrientRotationToMovement = false;

	//初期速度
	GetCharacterMovement()->MaxWalkSpeed = m_WalkSpeed;

	//初期メッシュ
	if (m_defaultMesh)
	{
		GetMesh()->SetSkeletalMesh(m_defaultMesh);
		m_Capsule->SetCapsuleSize(m_capsuleRadius, m_capsuleHeight);
		m_bUsingMesh = true;
	}

	m_pSpringArm->SetRelativeRotation(m_cameraInitPos[(int)ECameraStatus::ThirdPerson].rotator);
	m_pCamera->FieldOfView = m_cameraInitPos[(int)ECameraStatus::ThirdPerson].fieldOfView;

	//PlayerCotrollerからEnhancedInputSubSystemを取得
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		//カメラピッチの制限
		PC->PlayerCameraManager->ViewPitchMin = -60.f;
		PC->PlayerCameraManager->ViewPitchMax = 20.f;
	}

	UCapsuleComponent* capsel = GetCapsuleComponent();
	//イベントを発生させるかtrue
	capsel->SetGenerateOverlapEvents(true);
	//コールバック関数登録
	//コリジョンプリセットをプレイヤー
	capsel->SetCollisionProfileName(TEXT("Player"));

	//コールバック関数を登録
	capsel->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlap);
	capsel->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlap);

	////カメラのkより
	//m_pSpringArm->TargetArmLength = m_cameraInitPos[(int)ECameraStatus::ThirdPerson].springArmLength;

	//毎フレームライトマネージャーを取得するのは無駄なのでBeginPlayで取得しておく
	m_pExtendedSpotLightManager = GetWorld()->GetSubsystem<UExtendedSpotLightManager>();
	//上記のエネミーマネージャーバージョン
	m_pEnemyManager = GetWorld()->GetSubsystem<UEnemyManager>();


	//ソード攻撃コンポーネントの初期化
	if (m_sword)
	{
		//念のため衝突対象クリア
		m_sword->ClearCollisionObjectType();
		//コリジョンpreset名の設定
		m_sword->SetProfileName(FName("PlayerWeapon"));
		//攻撃範囲の設定
		m_sword->SetSwordAttackScale(m_attackRadius);
		//攻撃時間の設定
		m_sword->SetAttackTime(1.f);
		//振り終わりのコールバック関数登録
		m_sword->RegisterSwingEndCallback(CreateSwingEndCallback(APlayerCharacter::OnAttackEnd));
	}

	//コンテニューしていれば情報取得
		//プレイデータの取得
	UPlayDataGameInstanceSubsystem* pPlayData = GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>();
	UCheckpointManager* pCheckMng = GetWorld()->GetSubsystem<UCheckpointManager>();
	if (!pPlayData) { return; }
	if (pPlayData->IsIsContinued()&&pPlayData->GetCheckpointInfo().Index!=-1) {
		m_playerInfo = pPlayData->GetPlayerInfo();
		//チェックポイントを一つでも触れていれば
		if (pCheckMng->GetCurrentCheckpointIndex() >= 0)
		{
			SetActorLocation(pCheckMng->GetCurrentCheckpointLocation());
		}
	}

	if (Controller)
	{
		Controller->SetControlRotation(GetActorRotation());
	}
}

//----------------------------------------------------------
// Tick（毎フレーム更新）
//----------------------------------------------------------
void APlayerCharacter::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	//デバッグ確認用
	if (m_cameraSwitchIA == nullptr) { return; }

	//カメラの更新処理
	UpdateCamera(_deltaTime);
	//カメラと遮蔽物のコリジョン判定
	UpdateCameraCollision();
	//敵に見つかっているかとかの更新
	UpdateCheckEnemyDetection();
	//無敵時間の更新処理
	UpdateInvincibleTime(_deltaTime);
	//ダメージ処理
	UpdateDamaged();
	//影状態
	UpdateShadow(_deltaTime);
	//ジャンプ更新処理
	UpdateJump(_deltaTime);


	//視点変更
	ViewpointSwitching(_deltaTime);

}

//----------------------------------------------------------
// 入力バインド
//----------------------------------------------------------
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent→EnhancedInputComponent変換
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInput) { return; }

	//X軸
	EnhancedInput->BindAction(m_moveIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_Move);
	//ダッシュ
	EnhancedInput->BindAction(m_moveDashIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveDash);
	//しゃがみ
	EnhancedInput->BindAction(m_moveCrouchIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_MoveCrouch);
	//ジャンプ
	EnhancedInput->BindAction(m_moveJumpIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveJump);

	//攻撃
	EnhancedInput->BindAction(m_attackIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_Attack);
	//武器変更
	EnhancedInput->BindAction(m_changeWeaponIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_changeWeapon);
	//影潜り
	EnhancedInput->BindAction(m_inShadowIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_InShadow);
	//インタラクト
	EnhancedInput->BindAction(m_interactIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_Interact);

	//カメラPitch
	EnhancedInput->BindAction(m_cameraPitchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraPitch);
	//カメラYaw
	EnhancedInput->BindAction(m_cameraYawIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraYaw);
	//カメラReset
	EnhancedInput->BindAction(m_cameraResetIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraReset);
	//視点変更
	EnhancedInput->BindAction(m_cameraSwitchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraSwitch);

	//ダッシュ
	EnhancedInput->BindAction(m_moveDashIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_MoveDash);

	//カメラPitch
	EnhancedInput->BindAction(m_cameraPitchIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_CameraPitchReleased);
	//カメラYaw
	EnhancedInput->BindAction(m_cameraYawIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_CameraYawReleased);
}

//----------------------------------------------------------
// カメラ更新
//----------------------------------------------------------
void APlayerCharacter::UpdateCamera(float _deltaTime)
{
	//視点切り替え中は処理しない
	if (m_bCameraSwitching) { return; }

	//カメラのステータスが俯瞰なら処理しない
	if (m_cameraStatus == ECameraStatus::TopDownView) { return; }

	//処理落ちしても一定速度でカメラが回るように補正
	float rotateCorrection = CGameUtility::GetFpsCorrection(_deltaTime);

	AddControllerYawInput(m_cameraRotateInput.X);
	AddControllerPitchInput(m_cameraRotateInput.Y);
}

//----------------------------------------------------------
// カメラ衝突判定処理
//----------------------------------------------------------
void APlayerCharacter::UpdateCameraCollision()
{
	//建物が透過されるのは除先生が作ったクラスの機能だから一旦放置
	//相談中
}

//----------------------------------------------------------
//アイドル状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateIdle()
{
	//ここは仮だから変えて
	if (!m_charaMoveInput.IsZero())
	{
		ChangePlayerStatus(EPlayerStatus::Walk);
	}
}

//----------------------------------------------------------
// 移動処理
//----------------------------------------------------------
void APlayerCharacter::UpdateMove(const bool _bInShadow /*= false*/)
{
	// 地面にいなければ何もしない
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}

	// 入力なし
	if (m_charaMoveInput.IsNearlyZero(0.1f))
	{
		return;
	}

	// 操作不可
	if (!m_bCanControl)
	{
		return;
	}

	float BaseYaw = 0.f;

	switch (m_cameraStatus)
	{
	case ECameraStatus::ThirdPerson:
	case ECameraStatus::InShadow:
		// TPS / 影はカメラ基準
		BaseYaw = GetControlRotation().Yaw;
		break;

	case ECameraStatus::TopDownView:
		// 俯瞰はキャラ基準
		BaseYaw = GetActorRotation().Yaw;
		break;

	default:
		BaseYaw = GetControlRotation().Yaw;
		break;
	}

	const FRotator BaseRot(0.f, BaseYaw, 0.f);

	// 前後左右ベクトル（※ ActorForwardVector 禁止）
	const FVector Forward = FRotationMatrix(BaseRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(BaseRot).GetUnitAxis(EAxis::Y);

	// 入力から移動方向を生成
	FVector MoveDir =
		Forward * m_charaMoveInput.Y +
		Right * m_charaMoveInput.X;

	MoveDir.Z = 0.f;

	if (!MoveDir.Normalize())
	{
		return;
	}

	// 移動
	AddMovementInput(MoveDir, 1.f);

	// 回転（移動方向に向かせる）
	// ※ カメラ切り替え中は回さない
	if (!m_bCameraSwitching)
	{
		const FRotator TargetRot = MoveDir.Rotation();

		const FRotator NewRot = FMath::RInterpTo(
			GetActorRotation(),
			TargetRot,
			GetWorld()->GetDeltaSeconds(),
			10.f
		);
		SetActorRotation(NewRot);
	}
}



//----------------------------------------------------------
//しゃがみ状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateCrouch()
{
	if (!m_bIsCrouch)
	{
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight / 2.f);
		GetCharacterMovement()->MaxWalkSpeed = m_CrouchSpeed;
		m_bIsCrouch = true;

		//カメラ距離を変更
		m_pSpringArm->TargetArmLength = 150.f;					//カメラ距離
		m_pSpringArm->SocketOffset = FVector(0.f, 25.f, 30.f);	//肩越し視点設定
	}
	else
	{
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight);
		GetCharacterMovement()->MaxWalkSpeed = m_WalkSpeed;
		m_bIsCrouch = false;

		//カメラ距離を変更
		m_pSpringArm->TargetArmLength = 300.f;					//カメラ距離
		m_pSpringArm->SocketOffset = FVector(0.f, 50.f, 60.f);	//肩越し視点設定
	}
}

//----------------------------------------------------------
//攻撃状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateAttack()
{
	//攻撃が終わればアイドルに
	if (!m_bCanAttack)
	{
		ChangePlayerStatus(EPlayerStatus::Idle);
		return;
	}
}

//----------------------------------------------------------
//ダメージ状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateDamaged()
{
	if (m_status != EPlayerStatus::Damage) { return; }


	//一定時間経過したらアイドルに戻す
	//無敵時間タイマーはダメージ処理時起動するため使用
	if (m_invincibleTimer > m_damageTime)
	{
		ChangePlayerStatus(EPlayerStatus::Idle);
		m_knockBackVelocity = FVector::ZeroVector;
		m_bCanControl = true;
	}
}

//----------------------------------------------------------
//やられ状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateDead(float _deltaTime)
{

}

//----------------------------------------------------------
//影状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateShadow(float _deltaTime)
{

	if (m_status != EPlayerStatus::InShadow) {
		if (m_shadowTimer > 0.f)
		{
			m_shadowTimer -= _deltaTime;
			if (m_shadowTimer < 0.f) { return; }
		}
		return;
	}


	if (m_bUsingMesh)
	{

		GetMesh()->SetSkeletalMesh(m_isShadowMesh);
	
		m_bUsingMesh = false;
		//UE_LOG(LogTemp, Log, TEXT("Mesh Chaged Shadow !!"));
	}


	m_shadowTimer += GetWorld()->GetDeltaSeconds();

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		const FName Profile = Capsule->GetCollisionProfileName();
		UE_LOG(LogTemp, Warning, TEXT("CollisionProfile=%s"), *Profile.ToString());
	}


	//影状態時間が最大時間を超えたらアイドル状態に戻す
	if (m_shadowTimer > m_maxShadowTime) {
		TransformationShadowToIdle(true);
		return;
	}
	bool bLightHit = m_pExtendedSpotLightManager->IsHitAllLight(GetFeetLocation());
	//足元が光に照らされていたらアイドル状態に戻す
	if (bLightHit)
	{
		UE_LOG(LogTemp, Display, TEXT("LightHit"));
		TransformationShadowToIdle(true);
		return;
	}

	//敵の入っていた場合メインのライトではなく敵ライトだけと判定
	if(m_pExtendedSpotLightManager->IsHitEnemyLight(GetFeetLocation())&&m_bOnShadow)
	{
		TransformationShadowToIdle(true);		
		UE_LOG(LogTemp, Display, TEXT("LightHit"));

		return;
	}
	
}


//----------------------------------------------------------
//敵にばれているか判別しそれに応じた処理する関数
//----------------------------------------------------------
void APlayerCharacter::UpdateCheckEnemyDetection()
{
	if (!m_pEnemyManager) { return; }
	 m_pNearestEnemy = m_pEnemyManager->GetNearestEnemy(GetActorLocation(), m_attackRange);
	if (!m_pNearestEnemy) { return; }
	if (!m_pNearestEnemy->IsPlayerFound()) { return; }
	//ここでUIの呼び出し

}

//----------------------------------------------------------
// 無敵時間の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateInvincibleTime(float _deltaTime)
{
	if (!m_bInvincible) { return; }
	m_invincibleTimer += _deltaTime;
	if (m_invincibleTimer > m_invincibleTimeLimit)
	{
		m_bInvincible = false;
		m_invincibleTimer = 0.f;
	}
}

//---------------------------------------------------------
//ジャンプ中の更新処理
//---------------------------------------------------------
void APlayerCharacter::UpdateJump(float _deltaTime)
{
	if (!m_bJumping) { return; }

	m_jumpTimer += _deltaTime;

	FVector newVelocity = GetCharacterMovement()->Velocity;
	const float gravity = 10000.f;
	const float maxGaravity =-1800.f;
	newVelocity.Z -= FMath::Pow(gravity,m_jumpTimer);

	if (newVelocity.Z < maxGaravity)
	{
		newVelocity.Z = maxGaravity;
	}
	GetCharacterMovement()->Velocity = newVelocity;
}


//----------------------------------------------------------
// 視点切り替え
//----------------------------------------------------------
void APlayerCharacter::ViewpointSwitching(float _deltaTime)
{
	//視点変換中でなければ処理しない
	if (!m_bCameraSwitching) {
		return;
	}

	// まずは距離
	//引数は　現在地、目標値、デルタタイム、補間スピード
	float NewLength = FMath::FInterpTo(
		m_pSpringArm->TargetArmLength,
		m_cameraInitPos[(int)m_cameraStatus].springArmLength,
		_deltaTime,
		5.f
	);
	m_pSpringArm->TargetArmLength = NewLength;

	//現在の角度取得
	FRotator NewRotation = m_pSpringArm->GetRelativeRotation();

	//Yaw
	NewRotation.Yaw = FMath::FInterpTo(
		NewRotation.Yaw,
		m_cameraInitPos[(int)m_cameraStatus].rotator.Yaw,
		_deltaTime,
		5.f
	);

	//Pitch
	NewRotation.Pitch = FMath::FInterpTo(
		NewRotation.Pitch,
		m_cameraInitPos[(int)m_cameraStatus].rotator.Pitch,
		_deltaTime,
		5.f
	);

	//Roll
	NewRotation.Roll = FMath::FInterpTo(
		NewRotation.Roll,
		m_cameraInitPos[(int)m_cameraStatus].rotator.Roll,
		_deltaTime,
		5.f
	);

	m_pSpringArm->SetRelativeRotation(NewRotation);

	//視野角
	float newFieldOfView= m_pCamera->FieldOfView;
	newFieldOfView = FMath::FInterpTo(
		newFieldOfView,
		m_cameraInitPos[(int)m_cameraStatus].fieldOfView,
		_deltaTime,
		5.f
	);
	m_pCamera->FieldOfView = newFieldOfView;



	//一定距離以下になれば目標地点と同じ座標にする
	//目標地点との差
	const float distanceSpringArm = m_pSpringArm->TargetArmLength - m_cameraInitPos[(int)m_cameraStatus].springArmLength;
	const float distanceYaw = abs(NewRotation.Yaw) - abs(m_cameraInitPos[(int)m_cameraStatus].rotator.Yaw);
	const float distancePitch = abs(NewRotation.Pitch) - abs(m_cameraInitPos[(int)m_cameraStatus].rotator.Pitch);
	const float distanceFieldOfView = abs(newFieldOfView) - abs(m_cameraInitPos[(int)m_cameraStatus].fieldOfView);

	//
	if (
		(abs(distanceSpringArm) < 1.f
		&& abs(distanceYaw) < 1.f 
		&& abs(distancePitch) < 1.f 
		&& abs(distanceFieldOfView) < 1.f
			)
		|| m_status == EPlayerStatus::Dead
		)
	{
		m_pSpringArm->TargetArmLength = m_cameraInitPos[(int)m_cameraStatus].springArmLength;

		NewRotation.Yaw = m_cameraInitPos[(int)m_cameraStatus].rotator.Yaw;
		NewRotation.Pitch = m_cameraInitPos[(int)m_cameraStatus].rotator.Pitch;
		m_pSpringArm->SetRelativeRotation(NewRotation);

		//視点切り替え中フラグをfalseに
		m_bCameraSwitching = false;
	}
}

//-----------------------------------------------------
//攻撃終了処理
//-----------------------------------------------------
void APlayerCharacter::OnAttackEnd()
{
	m_bCanAttack = false;
	m_bCanControl = true;
	m_attackCount = 0.f;
}


//------------------------------------------------------
// ジャンプ終了処理
//------------------------------------------------------
void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// 着地時の処理
	UE_LOG(LogTemp, Warning, TEXT("Player has landed!"));

	m_bJumping = false;
	m_jumpTimer = 0.f;
	GetCharacterMovement()->GravityScale = 1.f;

}

//-----------------------------------------------------
//ダメージ処理
//-----------------------------------------------------
void APlayerCharacter::OnDamage(int32 Damage, FVector KnockBackVec, bool bSneakKill)
{

	//無敵時間中ならダメージを受けない
	if (m_bInvincible) { return; }
	
	
	//ダメージ処理
	//UE_LOG(LogTemp, Log, TEXT("Player Damaged : %d"), _damage);
	UE_LOG(LogTemp, Display, TEXT("hp："), m_playerInfo.hp);

	//入力制御
	m_bCanControl = false;
	//ダメージ
	m_playerInfo.hp -= Damage;
	
	//影状態なら解除
	CancellationShadow(EPlayerStatus::Damage);

	FVector& Vec = GetCharacterMovement()->Velocity;
	if (Vec.Z > 0)
	{
		Vec.Z = 0.f;
	}


	//Hpが0になったら死亡
	if (m_playerInfo.hp<=0)
	{
		m_bCanControl = false;
		m_playerInfo.bIsGetKeyItem = 0;
		m_playerInfo.isAlive = false;
		ChangePlayerStatus(EPlayerStatus::Dead);

		// クリア用レベルへ遷移
		UGameplayStatics::OpenLevel(this, FName("GameOver")); // レベル名を設定

		return;
	}

	/*生きていれば*/
	//無敵時間開始
	m_bInvincible = true;
	m_invincibleTimer = 0.f;
	ChangePlayerStatus(EPlayerStatus::Damage);
	m_knockBackVelocity = KnockBackVec;
	if (m_cameraStatus != ECameraStatus::ThirdPerson) {
		m_bCameraSwitching = true;
		m_cameraStatus = ECameraStatus::ThirdPerson;
	}

	LaunchCharacter(m_knockBackVelocity, true, false);

	UE_LOG(LogTemp, Display, TEXT("hp："),m_playerInfo.hp);
}


//-----------------------------------------------------
//状態変更処理
//-----------------------------------------------------
void APlayerCharacter::ChangePlayerStatus(const EPlayerStatus& _newStatus)
{
	m_status = _newStatus;
	//状態変更されたことを通知
	OnPlayerConditionMet.Broadcast(_newStatus);
}


//-----------------------------------------------------
//影状態か
//-----------------------------------------------------
bool APlayerCharacter::IsInShadow()const
{
	return m_status == EPlayerStatus::InShadow;
}


//-------------------------------------------------------
//足元座標取得
//-------------------------------------------------------
FVector APlayerCharacter::GetFeetLocation() const
{
	//影の上(足元が光に照らされていなければ影状態に
	FVector targetPos = GetActorLocation();
	const float capselHeighiHalf = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	targetPos.Z -= capselHeighiHalf;
	return targetPos;
}

//------------------------------------------------------
//プレイヤー情報取得
//------------------------------------------------------
FPlayerInfo APlayerCharacter::GetPlayerInfo()
{
	return m_playerInfo;
}


//------------------------------------------------------
//移動
//------------------------------------------------------
void APlayerCharacter::Enhanced_Move(const FInputActionValue& Value)
{
	//受け取る型にキャスト？指定して代入
	m_charaMoveInput = Value.Get<FVector2D>();
	//スティックの問題なのか、UE側の問題なのかわからんが、入力なくても0.01位の値が入っちゃてるので補正
	if (abs(m_charaMoveInput.Y) < 0.1f)
	{
		m_charaMoveInput.Y = 0.f;
	}
	if (abs(m_charaMoveInput.X) < 0.1f)
	{
		m_charaMoveInput.X = 0.f;
	}

	UpdateMove();
}

//------------------------------------------------------
//ダッシュ
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveDash(const FInputActionValue& Value)
{
	//しゃがみ中ならダッシュはできない
	if (m_bIsCrouch || m_status == EPlayerStatus::InShadow) { return; }

	const bool flag = Value.Get<bool>();

	//移動速度をflagで切り替える
	GetCharacterMovement()->MaxWalkSpeed = flag ? m_DashSpeed : m_WalkSpeed;


	//デバック用
	UNoiseManager* manager = GetWorld()->GetSubsystem<UNoiseManager>();
	if (manager)
	{
		manager->MakeNoise(3, GetActorLocation());
	}
}

//------------------------------------------------------
//しゃがみ
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveCrouch(const FInputActionValue& Value)
{
	if (m_status == EPlayerStatus::InShadow) {
		return;
	}
	if (!m_bIsCrouch)
	{
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight / 2.f);
		GetCharacterMovement()->MaxWalkSpeed = m_CrouchSpeed;
		m_bIsCrouch = true;
		m_cameraStatus = ECameraStatus::Crouch;
		m_bCameraSwitching=true;
	}
	else
	{
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight);
		GetCharacterMovement()->MaxWalkSpeed = m_WalkSpeed;
		m_bIsCrouch = false;
		m_cameraStatus = ECameraStatus::ThirdPerson;
		m_bCameraSwitching = true;

	}
}

//------------------------------------------------------
//ジャンプ
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveJump(const FInputActionValue& Value)
{
	//入力がないなら何もしない
	if (!Value.Get<bool>()) { return; }
	if (!m_bCanControl) { return; }
	if (m_status == EPlayerStatus::InShadow) { return; }

	//地面に接地しているときだけ
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		//慣性は残すが減速
		//GetCharacterMovement()->Velocity *= 0.8f;
		LaunchCharacter(FVector(0.f, 0.f, m_JumpVector), false, true);
		GetCharacterMovement()->GravityScale = 0.f;
		m_bJumping = true;
	}

	//デバック用
	UNoiseManager* manager = GetWorld()->GetSubsystem<UNoiseManager>();
	if (manager)
	{
		manager->MakeNoise(3, GetActorLocation());
	}
}

//------------------------------------------------------
//攻撃
//------------------------------------------------------
void APlayerCharacter::Enhanced_Attack(const FInputActionValue& Value)
{
	//プレイヤーがコントロールできなければ何もしない
	if (m_bCanControl == false) { return; }
	//ジャンプ中は攻撃できない
	if (m_bJumping) { return; }
	//影状態なら攻撃できない
	if (m_status == EPlayerStatus::InShadow) { return; }

	GetCharacterMovement()->GravityScale = 0.1f;

	m_bCanControl = false;
	ChangePlayerStatus(EPlayerStatus::Attack);
	m_bCanAttack=true;
	//一番近くの敵がプイレイヤーを見つけていればスニークキルするか判定
	//nullなら敵がいない
	if (m_pNearestEnemy) 
	{ 
		//みつかっていればスニークキル失敗
		if(m_pNearestEnemy->IsPlayerFound()) { 
			m_bSneakKill=false; 
		}
		else
		{
			//敵までのベクトル
			FVector toEnemyVector = FVector{m_pNearestEnemy->GetActorLocation()- GetActorLocation() }.GetSafeNormal();
			//プレイヤーの正面ベクトル
			FVector forwardVector =  GetMesh()->GetRightVector();     // = Y軸

			//ベクトルの差を-1~1の範囲で取得
			//1に近いほど正面同士
			const float dot = FVector::DotProduct(toEnemyVector, forwardVector);
			const float minAngle = 0.4f;
			if (dot > minAngle)
			{
				m_bSneakKill = true;
			}
			else
			{
				m_bSneakKill = false;
			}
		}
	}

	else
	{
		m_bSneakKill = false;
	}
	//剣の攻撃処理
//------------この前に振れる状態か確認-----------------
	m_sword->Swinging(m_bSneakKill);
}

//------------------------------------------------------
//影潜り
//------------------------------------------------------
void APlayerCharacter::Enhanced_InShadow(const FInputActionValue& Value)
{
	//入力が無ければ何もしない
	//if (!Value.Get<bool>()) { return; }	

	if (m_status == EPlayerStatus::Damage) { return; }
	
	//状態が影ならデフォルトに戻す
	if (m_status == EPlayerStatus::InShadow)
	{
		TransformationShadowToIdle();
		return;
	}


	//足元がライトに当たっていなければ影状態へ
	if (!m_pExtendedSpotLightManager->IsHitAllLight(GetFeetLocation())
		||(m_bOnShadow
		&&!m_pExtendedSpotLightManager->IsHitEnemyLight(GetFeetLocation()))) 
	{
	
		TransformationToShadow();
	}
}

//------------------------------------------------------
//インタラクト
//------------------------------------------------------
void APlayerCharacter::Enhanced_Interact(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Input Interact"));
	//インタラクト可能オブジェクトに触れていなければ何もしない
	if (!m_bHitIntteractObject) { return; }
	if (m_hitInteractOb)
	{
		m_hitInteractOb->Interact((AActor*)this);
	}
}

//------------------------------------------------------
//武器変更
//------------------------------------------------------
void APlayerCharacter::Enhanced_changeWeapon(const FInputActionValue& Value)
{

}

//------------------------------------------------------
//カメラPitch
//------------------------------------------------------
void APlayerCharacter::Enhanced_CameraPitch(const FInputActionValue& Value)
{
	m_cameraRotateInput.Y = -Value.Get<float>();

	//デッドゾーン
	if (abs(m_cameraRotateInput.Y) < 0.1f)
	{
		m_cameraRotateInput.Y = 0.f;
	}
}

//------------------------------------------------------
//カメラYaw
//------------------------------------------------------
void APlayerCharacter::Enhanced_CameraYaw(const FInputActionValue& Value)
{
	m_cameraRotateInput.X = Value.Get<float>();

	//デッドゾーン
	if (abs(m_cameraRotateInput.X) < 0.1f)
	{
		m_cameraRotateInput.X = 0.f;
	}
}

//------------------------------------------------------
//カメラReset
//------------------------------------------------------
void APlayerCharacter::Enhanced_CameraReset(const FInputActionValue& Value)
{
	//入力がなければ何もしない
	if (!Value.Get<bool>()) { return; }

	//プレイヤーの向きを取得
	FRotator targetRot = GetActorRotation();

	//Yawを合わせる
	FRotator NewControlRot = Controller->GetControlRotation();
	NewControlRot.Yaw = targetRot.Yaw;

	Controller->SetControlRotation(NewControlRot);
}

//----------------------------------------------------------
//カメラ視点変更
//----------------------------------------------------------
void APlayerCharacter::Enhanced_CameraSwitch(const FInputActionValue& Value)
{
	//視点変更中フラグON(trueしかかえって来ない様設定してる
	m_bCameraSwitching = Value.Get<bool>();
	m_cameraStatus = (m_cameraStatus == ECameraStatus::ThirdPerson) ? ECameraStatus::TopDownView : ECameraStatus::ThirdPerson;

	if (m_bCameraSwitching) {
		UE_LOG(LogTemp, Display, TEXT("trueeeeeeeeeeeeeeeeeeeeeeeeeee"));
	}
}

//-----------------------------------------------------------
//離された瞬間値を０にするよう
//カメラピッチ
//-------------------------------------------------------------
void APlayerCharacter::Enhanced_CameraPitchReleased(const FInputActionValue& Value)
{
	m_cameraRotateInput.Y = 0.f;
}

//-----------------------------------------------------------
//離された瞬間値を０にするよう
//カメラヨー
//-------------------------------------------------------------
void APlayerCharacter::Enhanced_CameraYawReleased(const FInputActionValue& Value)
{
	m_cameraRotateInput.X = 0.f;
}

//-----------------------------------------------
//衝突時のコールバック関数
//-----------------------------------------------
void APlayerCharacter::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	UE_LOG(LogTemp, Display, TEXT("unnti"));

	if (!OtherActor) { return; }
		//影オブジェクトと衝突したか
		if(OtherComp->ComponentHasTag(TEXT("Shadow"))==true)
		{
			//衝突対象を追加
			m_hitActors.Add(OtherActor);
			m_bOnShadow = true;
		}

	//インタラクト可能オブジェクトに触れた時
	if (OtherActor->ActorHasTag(TEXT("Interact")))
	{
		m_bHitIntteractObject = true;
		m_hitInteractOb = (AInteract*)OtherActor;
	}

}

//-------------------------------------------------
//衝突終了時のコールバック関数
//-------------------------------------------------
void APlayerCharacter::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	UE_LOG(LogTemp, Display, TEXT("end Hit"));
	if (!OtherActor) { return; }

	if (OtherComp->ComponentHasTag(TEXT("Shadow")))
	{
		//衝突対象を追加
		m_hitActors.Remove(OtherActor);
		if (m_hitActors.Num() <= 0)
			m_bOnShadow = false;
	}

	//インタラクト可能オブジェクトに触れた時
	if (OtherActor->ActorHasTag(TEXT("Interact")))
	{
		m_bHitIntteractObject = false;
		m_hitInteractOb = nullptr;
	}
}

//デバック用
void APlayerCharacter::OnNoiseHeard(const int& _noiseVolume, const FVector& _pos)
{
	m_bOnShadow = false;
}


//----------------------------------------------------------
// 影状態から通常状態へ変化
//----------------------------------------------------------
void APlayerCharacter::TransformationShadowToIdle(const bool _bLightHit/*=false*/)
{
	//カメラ戻す
	m_cameraStatus = ECameraStatus::ThirdPerson;
	m_bCameraSwitching = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));

	//ライトに当たっているなら普通に戻す
	if (!_bLightHit) {
		GetMesh()->SetSkeletalMesh(m_defaultMesh);
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight);
		m_bUsingMesh = false;
		ChangePlayerStatus(EPlayerStatus::Idle);
	}
	else
	{
		//ライトに当たっているなら通常状態へ、スタンみたいなの入れたい
		//一旦仮で普通に戻す
		GetMesh()->SetSkeletalMesh(m_defaultMesh);
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight);
		m_bUsingMesh = false;
		ChangePlayerStatus( EPlayerStatus::Idle);
	}
}

//----------------------------------------------------------
//影状態への変化
//----------------------------------------------------------
void APlayerCharacter::TransformationToShadow()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerInShadow"));

	ChangePlayerStatus(EPlayerStatus::InShadow);
	FVector newLocation = GetActorLocation();
	m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight / 3.f);
	newLocation.Z -= m_capsuleHeight / 3.f-10.f;
	SetActorLocation(newLocation);

	m_cameraStatus = ECameraStatus::InShadow;
	m_bCameraSwitching = true;

	m_bUsingMesh = true;

}

//影状態解除
void APlayerCharacter::CancellationShadow(const EPlayerStatus& _status)
{
	if (m_status != EPlayerStatus::InShadow) { return; }

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	
	GetMesh()->SetSkeletalMesh(m_defaultMesh);
	m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight);
	m_bUsingMesh = false;
	ChangePlayerStatus( _status);
	
}
