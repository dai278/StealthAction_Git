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
	, m_cameRotateSpeed(3.f)
	, m_WalkSpeed(600.f)
	, m_DashSpeed(1200.f)
	, m_CrouchSpeed(300.f)
	, m_JumpVector(600.f)
	, m_bCameraSwitching(false)
	, m_status(EPlayerStatus::Idle)
	, m_bCanControl(true)
	, DefaultMappingContext(nullptr)
	, m_moveVerticalIA(nullptr)
	, m_moveLateralIA(nullptr)
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
	, m_timer(0.f)
	, AttackRange(150.f)
	, AttackRadius(50.f)
	, AttackDamage(10.f)
	, bCanAttack(true)
	, AttackCooldown(0.3f)
	, m_pExtendedSpotLightManager(nullptr)

{
	//毎フレームTickを呼ぶか決めるフラグ
	PrimaryActorTick.bCanEverTick = true;

	//デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//動的配列TArrayの初期化
	m_pHitActors.Reset();

	//カメラの視点変更時初期位置
	m_cameraInitPos[(int)ECameraStatus::ThirdPerson] = { 0.f,0.f,0.f,800.f };
	m_cameraInitPos[(int)ECameraStatus::TopDownView] = { 0.f ,-80.f,0.f,2000.f };

	//カプセルの初期値を記録
	m_Capsule = GetCapsuleComponent();
	m_capsuleRadius = 40.f;
	m_capsuleHeight = 80.f;

	//スプリングアームのオブジェクトを生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL) {
		m_pSpringArm->SetupAttachment(RootComponent);

		//カメラのコリジョンテストを行うかを設定
		m_pSpringArm->bDoCollisionTest = false;
		//カメラ追従ラグを使うかを設定
		m_pSpringArm->bEnableCameraLag = true;
		//カメララグの追従速度決定
		m_pSpringArm->CameraLagSpeed = 10.0f;
		//カメラ回転ラグを使う決定
		m_pSpringArm->bEnableCameraRotationLag = true;
		//カメラ回転ラグの速度を決定	
		m_pSpringArm->CameraRotationLagSpeed = 5.0f;
	}

	//カメラオブジェクトを設定
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if ((m_pCamera != nullptr) && (m_pSpringArm != nullptr)) {
		//カメラをスプリングアームにタッチさせる
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
	}
}

//----------------------------------------------------------
// BeginPlay（ゲーム開始時）
//----------------------------------------------------------
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//初期速度
	GetCharacterMovement()->MaxWalkSpeed = m_WalkSpeed;

	//初期メッシュ
	if (m_defaultMesh)
	{
		GetMesh()->SetSkeletalMesh(m_defaultMesh);
		m_Capsule->SetCapsuleSize(m_capsuleRadius, m_capsuleHeight);
		m_bUsingMesh = true;
	}

	//PlayerCotrollerからEnhancedInputSubSystemを取得
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
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
}

//----------------------------------------------------------
// Tick（毎フレーム更新）
//----------------------------------------------------------
void APlayerCharacter::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	//デバッグ確認用
	if (m_cameraSwitchIA == nullptr) { return; }

	//ステータスによって処理を分岐
	switch (m_status)
	{
		//アイドル
	case EPlayerStatus::Idle:
		UpdateIdle(_deltaTime);
		break;
		//移動
	case EPlayerStatus::Walk:
		UpdateMove(_deltaTime);
		break;
		//しゃがみ
	case EPlayerStatus::Crouch:
		UpdateCrouch(_deltaTime);
		break;
		//攻撃
	case EPlayerStatus::Attack:
		UpdateAttack(_deltaTime);
		break;
		//ダメージ
	case EPlayerStatus::Damage:
		UpdateDamaged(_deltaTime);
		break;
		//やられ
	case EPlayerStatus::Dead:
		UpdateDead(_deltaTime);
		break;
		//影潜り
	case EPlayerStatus::InShadow:
		UpdateShadow(_deltaTime);
		break;
	}

	//カメラの更新処理
	UpdateCamera(_deltaTime);
	//カメラと遮蔽物のコリジョン判定
	UpdateCameraCollision();

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
	EnhancedInput->BindAction(m_moveVerticalIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveVertical);
	//Y軸
	EnhancedInput->BindAction(m_moveLateralIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveLateral);
	//ダッシュ
	EnhancedInput->BindAction(m_moveDashIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveDash);
	//しゃがみ
	EnhancedInput->BindAction(m_moveCrouchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveCrouch);
	//ジャンプ
	EnhancedInput->BindAction(m_moveJumpIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveJump);

	//攻撃
	EnhancedInput->BindAction(m_attackIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_Attack);
	//武器変更
	EnhancedInput->BindAction(m_changeWeaponIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_changeWeapon);
	//影潜り
	EnhancedInput->BindAction(m_inShadowIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_InShadow);

	//カメラPitch
	EnhancedInput->BindAction(m_cameraPitchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraPitch);
	//カメラYaw
	EnhancedInput->BindAction(m_cameraYawIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraYaw);
	//カメラReset
	EnhancedInput->BindAction(m_cameraResetIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraReset);
	//視点変更
	EnhancedInput->BindAction(m_cameraSwitchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraSwitch);

	//離した時に０の値が来るようにする
	//移動X
	EnhancedInput->BindAction(m_moveVerticalIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_MoveVerticalReleased);
	//移動Y
	EnhancedInput->BindAction(m_moveLateralIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_MoveLateralReleased);
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

	//カメラの新しい角度を求める
	//現在の角度を取得
	FRotator NewRotation = m_pSpringArm->GetRelativeRotation();

	//Yaw
	NewRotation.Yaw += m_cameraRotateInput.X * rotateCorrection * m_cameRotateSpeed;

	//Pitchに関しては上下の制限角度ない
	//FMath::Clampは(値,最小値,最大値
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + (m_cameraRotateInput.Y * rotateCorrection), m_cameraPitchLimit.X, m_cameraPitchLimit.Y);

	//新しい角度の反映
	m_pSpringArm->SetRelativeRotation(NewRotation);
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
void APlayerCharacter::UpdateIdle(float _deltaTime)
{
	//ここは仮だから変えて
	if (!m_charaMoveInput.IsZero())
	{
		m_status = EPlayerStatus::Walk;
	}
}

//----------------------------------------------------------
// 移動処理
//----------------------------------------------------------
void APlayerCharacter::UpdateMove(float _deltaTime)
{
	//移動入力がある場合
	if (!m_charaMoveInput.IsZero()) {
		//コントロール不可であれば何もしない
		if (m_bCanControl == false) {
			return;
		}

		//キャラクタークラスについている移動制御コンポーネントを使いキャラクターを移動させる			
		FVector forwardVec;
		FVector rightVec;

		//キャラクターの移動
		{
			// --- TPS と TopDown で使う基準が違う ---
			if (m_cameraStatus == ECameraStatus::ThirdPerson)
			{
				// 三人称はカメラ方向に移動
				forwardVec = m_pSpringArm->GetForwardVector();
				rightVec = m_pSpringArm->GetRightVector();
			}
			else
			{
				// 俯瞰はActorの正面方向を基準に移動
				forwardVec = GetActorForwardVector();
				rightVec = GetActorRightVector();
			}

			// 実際の移動
			AddMovementInput(forwardVec, m_charaMoveInput.Y);
			AddMovementInput(rightVec, m_charaMoveInput.X);

			//デバック用
			UNoiseManager* manager = GetWorld()->GetSubsystem<UNoiseManager>();
			if (manager)
			{
				manager->MakeNoise(1, GetActorLocation());
			}
		}

		//移動するキャラクターを回転
		{
			//角度はDegreeの角度の範囲で表記(-180~180)
			USkeletalMeshComponent* pMeshComp = GetMesh();
			if (pMeshComp == NULL) { return; }
			//アークタンジェントを使ってコントローラの入力方向がなす角度を求める
			float angle = atan2(m_charaMoveInput.X, m_charaMoveInput.Y);
			//Radian値にDegreeに変換
			float angleDeg = FMath::RadiansToDegrees(angle);

			float newYaw = 0.f;
			//三人称なら
			if (m_cameraStatus == ECameraStatus::ThirdPerson)
			{
				//入力した角度＋メッシュの回転角度＋Actorに対して回転しているSpringArmの相対角度
				newYaw = angleDeg + GetBaseRotationOffsetRotator().Yaw + m_pSpringArm->GetRelativeRotation().Yaw;
			}
			else
			{
				//カメラ方向は関係なし、Actorを基準に向きを決める
				newYaw = angleDeg + GetActorRotation().Yaw;
			}
			pMeshComp->SetRelativeRotation(FRotator(pMeshComp->GetRelativeRotation().Pitch, newYaw, pMeshComp->GetRelativeRotation().Roll));
		}
	}
}

//----------------------------------------------------------
//しゃがみ状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateCrouch(float _deltaTime)
{

}

//----------------------------------------------------------
//攻撃状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateAttack(float _deltaTime)
{
	if (!bCanAttack) return;
	bCanAttack = false;

	// ===== カメラ方向を取得 =====
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;   // 上下無視（地面攻撃用）
	ControlRot.Roll = 0.f;

	FVector AttackDir = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);

	FVector Start = GetActorLocation();
	FVector End = Start + AttackDir * AttackRange;
	// ==========================

	FCollisionShape Shape = FCollisionShape::MakeSphere(AttackRadius);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	TArray<FHitResult> Hits;

	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Shape,
		Params
	);

	// ===== デバッグ =====
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f);
	DrawDebugSphere(GetWorld(), End, AttackRadius, 12, FColor::Red, false, 1.f);
	// ====================

	if (bHit)
	{
		for (const FHitResult& Hit : Hits)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				UGameplayStatics::ApplyDamage(
					HitActor,
					AttackDamage,
					GetController(),
					this,
					nullptr
				);
			}
		}
	}

	// クールタイム
	GetWorldTimerManager().SetTimer(
		AttackCooldownHandle,
		[this]()
		{
			bCanAttack = true;
		},
		AttackCooldown,
		false
	);
}

//----------------------------------------------------------
//ダメージ状態の更新
//----------------------------------------------------------
void APlayerCharacter::UpdateDamaged(float _deltaTime)
{

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
	if (m_bUsingMesh)
	{
		GetMesh()->SetSkeletalMesh(m_isShadowMesh);
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight / 3.f);
		m_bUsingMesh = false;
		//UE_LOG(LogTemp, Log, TEXT("Mesh Chaged Shadow !!"));
	}
	else
	{
		m_bUsingMesh = true;
	}

	m_timer += _deltaTime;

	//影状態時間が最大時間を超えたらアイドル状態に戻す
	if (m_timer > m_maxShadowTime) {
		TransformationShadowToIdle();
		return;
	}
	//足元が光に照らされていたらアイドル状態に戻す
	if (m_pExtendedSpotLightManager->IsHitLight(GetFeetLocation()))
	{
		TransformationShadowToIdle(true);
		return;
	}
	//移動処理
	UpdateMove(_deltaTime);
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
		m_cameraInitPos[(int)m_cameraStatus].yaw,
		_deltaTime,
		5.f
	);

	//Pitch
	NewRotation.Pitch = FMath::FInterpTo(
		NewRotation.Pitch,
		m_cameraInitPos[(int)m_cameraStatus].pitch,
		_deltaTime,
		5.f
	);

	//Roll
	NewRotation.Roll = FMath::FInterpTo(
		NewRotation.Roll,
		m_cameraInitPos[(int)m_cameraStatus].roll,
		_deltaTime,
		5.f
	);

	m_pSpringArm->SetRelativeRotation(NewRotation);

	//一定距離以下になれば目標地点と同じ座標にする
	//目標地点との差
	const float distanceSpringArm = m_pSpringArm->TargetArmLength - m_cameraInitPos[(int)m_cameraStatus].springArmLength;
	const float distanceYaw = abs(NewRotation.Yaw) - abs(m_cameraInitPos[(int)m_cameraStatus].yaw);
	const float distancePitch = abs(NewRotation.Pitch) - abs(m_cameraInitPos[(int)m_cameraStatus].pitch);
	//
	if (
		abs(distanceSpringArm) < 1.f &&
		abs(distanceYaw) < 1.f &&
		abs(distancePitch) < 1.f
		)
	{
		m_pSpringArm->TargetArmLength = m_cameraInitPos[(int)m_cameraStatus].springArmLength;

		NewRotation.Yaw = m_cameraInitPos[(int)m_cameraStatus].yaw;
		NewRotation.Pitch = m_cameraInitPos[(int)m_cameraStatus].pitch;
		m_pSpringArm->SetRelativeRotation(NewRotation);

		//視点切り替え中フラグをfalseに
		m_bCameraSwitching = false;
	}
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
//Y軸移動
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveVertical(const FInputActionValue& Value)
{
	//受け取る型にキャスト？指定して代入
	m_charaMoveInput.Y = Value.Get<float>();
	//スティックの問題なのか、UE側の問題なのかわからんが、入力なくても0.01位の値が入っちゃてるので補正
	if (abs(m_charaMoveInput.Y) < 0.1f)
	{
		m_charaMoveInput.Y = 0.f;
	}
}

//------------------------------------------------------
//X軸移動
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveLateral(const FInputActionValue& Value)
{
	m_charaMoveInput.X = Value.Get<float>();
	//スティックの問題なのか、UE側の問題なのかわからんが、入力なくても0.01位の値が入っちゃてるので補正
	if (abs(m_charaMoveInput.X) < 0.1f)
	{
		m_charaMoveInput.X = 0.f;
	}
}

//------------------------------------------------------
//ダッシュ
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveDash(const FInputActionValue& Value)
{
	//しゃがみ中ならダッシュはできない
	if (m_bIsCrouch) { return; }

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
	if (!m_bIsCrouch)
	{
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight / 2.f);
		GetCharacterMovement()->MaxWalkSpeed = m_CrouchSpeed;
		m_bIsCrouch = true;
	}
	else
	{
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight);
		GetCharacterMovement()->MaxWalkSpeed = m_WalkSpeed;
		m_bIsCrouch = false;
	}
}

//------------------------------------------------------
//ジャンプ
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveJump(const FInputActionValue& Value)
{
	//入力がないなら何もしない
	if (!Value.Get<bool>()) { return; }

	//地面に接地しているときだけ
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		LaunchCharacter(FVector(0.f, 0.f, m_JumpVector), false, true);
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
	UpdateAttack();
}

//------------------------------------------------------
//影潜り
//------------------------------------------------------
void APlayerCharacter::Enhanced_InShadow(const FInputActionValue& Value)
{
	//入力が無ければ何もしない
	//if (!Value.Get<bool>()) { return; }

	

	//状態が影ならデフォルトに戻す
	if (m_status == EPlayerStatus::InShadow)
	{
		TransformationShadowToIdle();
		return;
	}

	//足元がライトに当たっていなければ影状態へ
	if (!m_pExtendedSpotLightManager->IsHitLight(GetFeetLocation())) {
		m_status = EPlayerStatus::InShadow;
		m_bUsingMesh = true;
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
//Y軸移動
//-------------------------------------------------------------
void APlayerCharacter::Enhanced_MoveVerticalReleased(const FInputActionValue& Value)
{
	m_charaMoveInput.Y = 0.f;
}
//-----------------------------------------------------------
//離された瞬間値を０にするよう
//X軸移動
//-------------------------------------------------------------
void APlayerCharacter::Enhanced_MoveLateralReleased(const FInputActionValue& Value)
{
	m_charaMoveInput.X = 0.f;
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
	if (OtherActor) {
		//衝突対象を追加
		m_hitActors.Add(OtherActor);
	}
	m_bOnShadow = true;
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
	if (OtherActor)
	{
		//衝突対象を減らす
		m_hitActors.Remove(OtherActor);
	}

	if (m_hitActors.Num() < 1) {
		m_bOnShadow = false;
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
	//ライトに当たっているなら普通に戻す
	if (!_bLightHit) {
		GetMesh()->SetSkeletalMesh(m_defaultMesh);
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight);
		m_bUsingMesh = false;
		m_status = EPlayerStatus::Idle;
		m_timer = 0.f;
	}
	else
	{
		//ライトに当たっているなら通常状態へ、スタンみたいなの入れたい
		//一旦仮で普通に戻す
		GetMesh()->SetSkeletalMesh(m_defaultMesh);
		m_Capsule->SetCapsuleHalfHeight(m_capsuleHeight);
		m_bUsingMesh = false;
		m_status = EPlayerStatus::Idle;
		m_timer = 0.f;
	}
}

//----------------------------------------------------------
//影状態への変化
//----------------------------------------------------------
void APlayerCharacter::TransformationToShadow()
{
	m_status = EPlayerStatus::InShadow;
	m_bUsingMesh = true;
	return;
}
