#include "PlayerCharacter.h"
#include "State/Player/PlayerCrouchState.h"
#include "State/Player/PlayerShadowState.h"
#include "State/Player/PlayerDashState.h"
//----------------------------------------------------------
// 概要				：プレイヤーキャラを制御するCharacterオブジェクト
// 更新日　　　　 　：11/19　
//----------------------------------------------------------

#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "State/Player/PlayerStateMachine.h"
#include "State/Player/PlayerAttackState.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
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

#include "Sword/SwordAttackComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/CollisionProfile.h"
#include "GameInstance/PlayDataInfo.h"
#include "GameInstance/PlayDataGameInstanceSubsystem.h"
#include "Checkpoint/CheckpointManager.h"
#include "Interact/Interact.h"
#include "Camera/CameraFocusDirectorComponent.h"
#include "UI/HUD/HUDWidget.h"
#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h"
#include "Components/ChildActorComponent.h"


//----------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------
APlayerCharacter::APlayerCharacter()
	: DefaultMappingContext(nullptr)
	, m_moveIA(nullptr)
	, m_moveJumpIA(nullptr)
	, m_moveDashIA(nullptr)
	, m_moveCrouchIA(nullptr)
	, m_cameraPitchIA(nullptr)
	, m_cameraYawIA(nullptr)
	, m_cameraResetIA(nullptr)
	, m_cameraSwitchIA(nullptr)
	, m_changeWeaponIA(nullptr)
	, m_attackIA(nullptr)
	, m_pSpringArm(nullptr)
	, m_pCamera(nullptr)
	, m_cameraStatus(ECameraStatus::ThirdPerson)
	, m_cameraPitchLimit(FVector2D(-80.f, 80.f))
	, m_cameraRotateSpeed(3.f)
	, m_pMPC(nullptr)
	, m_defaultMesh(nullptr)
	, m_Capsule(nullptr)
	, m_capsuleRadius(0.f)
	, m_capsuleHeight(0.f)
	, m_bCameraSwitching(false)
	, m_bCanControl(true)
	, m_bOnShadow(false)
	, m_charaMoveInput(FVector2D::ZeroVector)
	, m_cameraRotateInput(FVector2D::ZeroVector)
	, m_maxOffsetY(70.f)
	, m_initOffsetY(30.f)
	, m_playerInfo()

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

	// Shared camera modes; state-specific camera profiles live on each state BP.
	ThirdPersonCamera = { FRotator(0.f, 0.f, 0.f), 70.f, 400.f, FVector(0.f, 70.f, 20.f), 1.f };
	TopDownCamera = { FRotator(0.f, 0.f, 0.f), 90.f, 1000.f, FVector(0.f, 70.f, 20.f), 5.f };
	EventCamera = ThirdPersonCamera;

	//カプセルの初期値を記録
	m_Capsule = GetCapsuleComponent();
	m_capsuleRadius = 40.f;
	m_capsuleHeight = 80.f;

	//スプリングアームのオブジェクトを生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL) {
		m_pSpringArm->SetupAttachment(RootComponent);

		m_pSpringArm->TargetArmLength = ThirdPersonCamera.springArmLength;					//カメラ距離
		m_pSpringArm->SocketOffset = FVector(0, m_maxOffsetY, 20.f);	//肩越し視点設定
		m_pSpringArm->bUsePawnControlRotation = true;			//カメラ回転
		m_pSpringArm->bEnableCameraLag = true;
		m_pSpringArm->CameraLagSpeed = 100.f;
		m_pSpringArm->bDoCollisionTest = true;                 // 壁自動回避
	}

	//剣の攻撃コンポーネント生成
	m_sword = CreateDefaultSubobject<USwordAttackComponent>(TEXT("Sword"));
	if (IsValid(m_sword))
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

	m_ShadowEffectChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("ShadowEffectChild"));
	m_ShadowEffectChild->SetupAttachment(GetRootComponent()); // もしくは GetMesh()

	// ここで固定でクラス指定するなら
	// ShadowEffectChild->SetChildActorClass(AShadowEffectActor::StaticClass());

//カメラフォーカスディレクターコンポーネント生成
	m_pCameraFocusDirector = CreateDefaultSubobject<UCameraFocusDirectorComponent>(TEXT("CameraFocusDirector"));
}

//----------------------------------------------------------
// BeginPlay（ゲーム開始時）
//----------------------------------------------------------
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (!GetWorld() || !IsValid(GetCharacterMovement()) || !IsValid(GetMesh())
		|| !IsValid(m_Capsule) || !IsValid(m_pSpringArm) || !IsValid(m_pCamera))
	{
		m_bCanControl = false;
		UE_LOG(LogTemp, Error, TEXT("%s: Required player components are missing."), *GetName());
		return;
	}

	GetCharacterMovement()->bOrientRotationToMovement = false;

	//初期メッシュ
	if (m_defaultMesh)
	{
		GetMesh()->SetSkeletalMesh(m_defaultMesh);
		m_Capsule->SetCapsuleSize(m_capsuleRadius, m_capsuleHeight);
	}

	m_pSpringArm->SetRelativeRotation(ThirdPersonCamera.rotator);
	m_pCamera->FieldOfView = ThirdPersonCamera.fieldOfView;

	//PlayerCotrollerからEnhancedInputSubSystemを取得
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		//カメラピッチの制限
		if (IsValid(PC->PlayerCameraManager))
		{
			PC->PlayerCameraManager->ViewPitchMin = -60.f;
			PC->PlayerCameraManager->ViewPitchMax = 20.f;
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
	//m_pSpringArm->TargetArmLength = ThirdPersonCamera.springArmLength;

	//ソード攻撃コンポーネントの初期化
	if (IsValid(m_sword))
	{
		//念のため衝突対象クリア
		m_sword->ClearCollisionObjectType();
		//コリジョンpreset名の設定
		m_sword->SetProfileName(FName("PlayerWeapon"));
		//振り終わりのコールバック関数登録
		m_sword->RegisterSwingEndCallback(CreateSwingEndCallback(APlayerCharacter::OnAttackEnd));
	}

	//コンテニューしていれば情報取得
	//プレイデータの取得
	UPlayDataGameInstanceSubsystem* pPlayData = (GetWorld() && GetWorld()->GetGameInstance() ? GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>() : nullptr);
	UCheckpointManager* pCheckMng = (GetWorld() ? GetWorld()->GetSubsystem<UCheckpointManager>() : nullptr);
	if (pPlayData && pPlayData->IsIsContinued() && pPlayData->GetCheckpointInfo().Index != -1) {
		m_playerInfo = pPlayData->GetPlayerInfo();
		//チェックポイントを一つでも触れていれば
		if (pCheckMng && pCheckMng->GetCurrentCheckpointIndex() >= 0)
		{
			SetActorLocation(pCheckMng->GetCurrentCheckpointLocation());
		}
	}

	if (IsValid(m_ShadowEffectChild))
	{
		if (AActor* Effect = m_ShadowEffectChild->GetChildActor()) { Effect->SetActorHiddenInGame(true); }
	}

	if (IsValid(Controller))
	{
		Controller->SetControlRotation(GetActorRotation());
	}

	if (m_stateMachineClass)
	{
		m_stateMachine = NewObject<UPlayerStateMachine>(this, m_stateMachineClass);
		m_stateMachine->Initialize(this);
		if (!m_stateMachine->IsConfigured()) { m_bCanControl = false; }
	}
	else
	{
		m_bCanControl = false;
		UE_LOG(LogTemp, Error, TEXT("%s: Set Player State Machine Class in the player Blueprint."), *GetName());
	}

}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->Stop(); }
	Super::EndPlay(EndPlayReason);
}

//----------------------------------------------------------
// Tick（毎フレーム更新）
//----------------------------------------------------------
void APlayerCharacter::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	//カメラの更新処理
	UpdateCamera(_deltaTime);
	//カメラと遮蔽物のコリジョン判定
	UpdateCameraCollision();

	// Update only the active state registered by the machine Blueprint.
	if (IsValid(m_stateMachine))
	{
		m_stateMachine->Update(_deltaTime);
	}

	//視点変更
	ViewpointSwitching(_deltaTime);

	//現在位置で通常状態に戻ったら壁にのめり込むか
	if (!IsUpPosWall(GetFeetLocation()))
	{
		m_saveLastNotUpWallPos = GetActorLocation();
	}

	//カメラとの距離が近すぎるときはメッシュを消す
	const UCameraComponent* Cam = FindComponentByClass<UCameraComponent>();
	if (!IsValid(Cam) || !IsValid(GetMesh())) return;

	const float D = FVector::Distance(Cam->GetComponentLocation(), GetMesh()->GetComponentLocation());
	if (D < 150) {
		GetMesh()->SetHiddenInGame(true, true); 
	}
	else {
		GetMesh()->SetHiddenInGame(false, true);
	}
	
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
	if (IsValid(m_moveIA)) { EnhancedInput->BindAction(m_moveIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_Move); }
	if (IsValid(m_moveIA)) { EnhancedInput->BindAction(m_moveIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_EndMove); }
	if (IsValid(m_moveIA)) { EnhancedInput->BindAction(m_moveIA, ETriggerEvent::Canceled, this, &APlayerCharacter::Enhanced_EndMove); }
	//しゃがみ
	if (IsValid(m_moveCrouchIA)) { EnhancedInput->BindAction(m_moveCrouchIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_MoveCrouch); }
	//ジャンプ
	if (IsValid(m_moveJumpIA)) { EnhancedInput->BindAction(m_moveJumpIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveJump); }

	//攻撃
	if (IsValid(m_attackIA)) { EnhancedInput->BindAction(m_attackIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_Attack); }
	//武器変更
	if (IsValid(m_changeWeaponIA)) { EnhancedInput->BindAction(m_changeWeaponIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_changeWeapon); }
	//影潜り
	if (IsValid(m_inShadowIA)) { EnhancedInput->BindAction(m_inShadowIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_InShadow); }
	//インタラクト
	if (IsValid(m_interactIA)) { EnhancedInput->BindAction(m_interactIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_Interact); }

	//カメラPitch
	if (IsValid(m_cameraPitchIA)) { EnhancedInput->BindAction(m_cameraPitchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraPitch); }
	//カメラYaw
	if (IsValid(m_cameraYawIA)) { EnhancedInput->BindAction(m_cameraYawIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraYaw); }
	//カメラReset
	if (IsValid(m_cameraResetIA)) { EnhancedInput->BindAction(m_cameraResetIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraReset); }
	//視点変更
	if (IsValid(m_cameraSwitchIA)) { EnhancedInput->BindAction(m_cameraSwitchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraSwitch); }

	//ダッシュ
	if (IsValid(m_moveDashIA)) { EnhancedInput->BindAction(m_moveDashIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_OnInputDash); }
	//ダッシュ
	if (IsValid(m_moveDashIA)) { EnhancedInput->BindAction(m_moveDashIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_EndInputDash); }

	//カメラPitch
	if (IsValid(m_cameraPitchIA)) { EnhancedInput->BindAction(m_cameraPitchIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_CameraPitchReleased); }
	//カメラYaw
	if (IsValid(m_cameraYawIA)) { EnhancedInput->BindAction(m_cameraYawIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_CameraYawReleased); }
}

//----------------------------------------------------------
// カメラ更新
//----------------------------------------------------------
void APlayerCharacter::UpdateCamera(float _deltaTime)
{
	if (!IsValid(m_pSpringArm) || !IsValid(GetCharacterMovement())) { return; }

	//カメラのステータスが俯瞰なら処理しない
	if (m_cameraStatus == ECameraStatus::TopDownView) { return; }
	if (CanAttack()) { return; }
	//if (m_bCameraSwitching) { return; }

	//処理落ちしても一定速度でカメラが回るように補正
	float rotateCorrection = CGameUtility::GetFpsCorrection(_deltaTime);

	AddControllerYawInput(m_cameraRotateInput.X);
	AddControllerPitchInput(m_cameraRotateInput.Y);

	//影潜り中はカメラオフセットを変更しない
	const UPlayerStateBase* CameraState = IsValid(m_stateMachine)
		? Cast<UPlayerStateBase>(m_stateMachine->GetCurrentState()) : nullptr;
	if (CameraState && CameraState->UsesCameraOverride()) { return; }

	//X軸入力があり、移動入力がなければ
	//カメラオフセットを移動させ、左右どちらの肩越し視点に対応できるようにし
	//角の先を見やすくする
	if (m_cameraRotateInput.X != 0 && GetCharacterMovement()->Velocity.Length() <= GetCharacterMovement()->MaxWalkSpeed) {
		float moveOffsetDir = (m_cameraRotateInput.X > 0) ? -1.f : 1.f;

		//現在のオフセット位置
		float targetOffset = m_pSpringArm->SocketOffset.Y;
		//100スピード
		targetOffset += moveOffsetDir * 70.f * _deltaTime;

		//最大値チェック
		if (targetOffset > m_maxOffsetY)
		{
			targetOffset = m_maxOffsetY;
		}
		else if (targetOffset < -m_maxOffsetY)
		{
			targetOffset = -m_maxOffsetY;
		}

		m_pSpringArm->SocketOffset.Y = targetOffset;

		//入力がない時間計測用タイマーリセット
		m_OutcameraInputTimer = 0.f;
	}
	
	//初期オフセット位置に戻す処理
	else
	{
		//タイマー加算
		m_OutcameraInputTimer += _deltaTime;
		//このマジックナンバーより時間たっていれば処理する
		if (m_OutcameraInputTimer > 0.3f)
		{

			//現在のオフセット位置
			float socketOffsetPosY = m_pSpringArm->SocketOffset.Y;

			//現在位置と初期オフセットの差
			float socketLeght = socketOffsetPosY - m_initOffsetY;

			//初期位置にいなければ処理？
			if (!FMath::IsNearlyZero(abs(socketLeght)))
			{
				//中央+-がどっちか
				float moveOffsetDir = (socketOffsetPosY > m_initOffsetY) ? -1.f : 1.f;

				//ソケット位置移動
				socketOffsetPosY += moveOffsetDir * 2.f * _deltaTime;
				m_pSpringArm->SocketOffset.Y = socketOffsetPosY;

				//ある程度近づいたら合わせる
				if (abs(socketLeght) < 0.5f)
				{
					m_pSpringArm->SocketOffset.Y = m_initOffsetY;
				}

			}
		}
	}
}

//----------------------------------------------------------
//手振れ補正処理
//----------------------------------------------------------
void APlayerCharacter::UpdateCameraShake(float _deltaTime)
{
	if (!IsValid(m_pSpringArm)) { return; }

	//現在のソケットオフセット
	FVector currentSocketOffset = m_pSpringArm->SocketOffset;
}

//----------------------------------------------------------
// カメラ衝突判定処理
//----------------------------------------------------------
void APlayerCharacter::UpdateCameraCollision()
{
	//建物が透過されるのは除先生が作ったクラスの機能だから一旦放置
	//相談中
}

//スタミナ消費

//スタミナ回復

//----------------------------------------------------------
// 視点切り替え
//----------------------------------------------------------
void APlayerCharacter::ViewpointSwitching(float _deltaTime)
{
	const FCameraViewSetting& View = GetCameraViewSetting();
	if (!IsValid(m_pSpringArm) || !IsValid(m_pCamera)) { return; }

	//視点変換中でなければ処理しない
	if (!m_bCameraSwitching) {
		return;
	}
	//

	// まずは距離
	//引数は　現在地、目標値、デルタタイム、補間スピード
	float NewLength = FMath::FInterpTo(
		m_pSpringArm->TargetArmLength,
		View.springArmLength,
		_deltaTime,
		View.InterpSpeed
	);
	m_pSpringArm->TargetArmLength = NewLength;
	m_pSpringArm->SocketOffset = FMath::VInterpTo(m_pSpringArm->SocketOffset, View.ofSet, _deltaTime, View.InterpSpeed);

	//視野角
	float newFieldOfView = m_pCamera->FieldOfView;
	newFieldOfView = FMath::FInterpTo(
		newFieldOfView,
		View.fieldOfView,
		_deltaTime,
		View.InterpSpeed
	);
	m_pCamera->FieldOfView = newFieldOfView;

	if (m_cameraStatus == ECameraStatus::SneakKill||m_cameraStatus==ECameraStatus::ThirdPerson)
	{
		//回転
		//Yaw
		FRotator newRot = m_pSpringArm->GetRelativeRotation();
		newRot.Yaw = FMath::FInterpTo(
			newRot.Yaw,
			View.rotator.Yaw,
			_deltaTime,
			View.InterpSpeed
		);

		//Pitch
		newRot.Pitch = FMath::FInterpTo(
			newRot.Pitch,
			View.rotator.Pitch,
			_deltaTime,
			View.InterpSpeed
		);

		//Roll
		newRot.Roll = FMath::FInterpTo(
			newRot.Roll,
			View.rotator.Roll,
			_deltaTime,
			View.InterpSpeed
		);

		m_pSpringArm->SetRelativeRotation(newRot);
		
		//目的回転に近づいたら合わせる
		if (abs(newRot.Yaw - View.rotator.Yaw) < 1.f)
		{
			m_pSpringArm->SetRelativeRotation(View.rotator);
		}
	}

	//一定距離以下になれば目標地点と同じ座標にする
	//目標地点との差
	const float distanceSpringArm = m_pSpringArm->TargetArmLength - View.springArmLength;
	const float distanceFieldOfView = abs(newFieldOfView) - abs(View.fieldOfView);

	//
	if (
		(abs(distanceSpringArm) < 1.f
			&& abs(distanceFieldOfView) < 1.f
			&& m_pSpringArm->SocketOffset.Equals(View.ofSet, 1.f)
			)
		)
	{
		m_pSpringArm->TargetArmLength = View.springArmLength;

		m_pCamera->FieldOfView = View.fieldOfView;
		//視点切り替え中フラグをfalseに
		m_bCameraSwitching = false;

	}
}

//-----------------------------------------------------
//攻撃終了処理
//-----------------------------------------------------
void APlayerCharacter::OnAttackEnd()
{
	if (IsValid(m_stateMachine)) { m_stateMachine->NotifyAttackFinished(); }
}

//-----------------------------------------------------
//カメラフォーカス処理
//-----------------------------------------------------
void APlayerCharacter::StartCameraFocus(AActor* const _cameraActor, float _blendTime)
{
	if (!IsValid(_cameraActor) || !IsValid(m_pCameraFocusDirector)) { return; }

	if (!IsValid(Controller)) { return; }
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC) { return; }

	//カメラの切り替え前のアクターを保存
	m_pSaveCameraActor = PC->GetViewTarget();

	//カメラの切り替え
	const bool bCouldControl = m_bCanControl;
	bool isStat = false;
	m_pCameraFocusDirector->StartFocusByProviderActor(_cameraActor, isStat, PC);
	if (!isStat) { m_bCanControl = bCouldControl; return; }
	m_bCanControl = false;

	if (IsValid(m_stateMachine)) { m_stateMachine->CancelDash(); m_stateMachine->RefreshLocomotionState(); }

}

//------------------------------------------------------
//カメラフォーカス終了処理
//------------------------------------------------------
void APlayerCharacter::EndCameraFocus(const float& _blendTime)
{
	if (!IsValid(Controller)) { return; }
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC) { return; }
	PC->SetViewTargetWithBlend(IsValid(m_pSaveCameraActor) ? m_pSaveCameraActor : this, _blendTime);
	m_bCanControl = true;
}

//------------------------------------------------------
// ジャンプ終了処理
//------------------------------------------------------
void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (IsValid(m_stateMachine)) { m_stateMachine->NotifyLanded(); }
}

//------------------------------------------------------
//イベント状態のカメラフォーカス終了
//------------------------------------------------------
void APlayerCharacter::SetEventCameraChange(FCameraViewSetting _viewSetting)
{
	EventCamera = _viewSetting;
	m_bCameraSwitching = true;
	m_cameraStatus = ECameraStatus::SneakKill;
}

//-----------------------------------------------------
//ダメージ処理
//-----------------------------------------------------
void APlayerCharacter::OnDamage(int32 Damage, FVector KnockBackVec, bool bSneakKill)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->ReceiveDamage(Damage, KnockBackVec); }
}

//-----------------------------------------------------
//状態変更処理
//-----------------------------------------------------
//-----------------------------------------------------
//現在ステート取得
//-----------------------------------------------------
UPlayerStateBase* APlayerCharacter::GetPlayerState() const
{
	return IsValid(m_stateMachine) ? Cast<UPlayerStateBase>(m_stateMachine->GetCurrentState()) : nullptr;
}

//-----------------------------------------------------
//現在ステートのAnimBP用の状態取得
//-----------------------------------------------------
EPlayerStatus APlayerCharacter::GetPlayerStatus() const
{
	const UPlayerStateBase* State = GetPlayerState();
	return IsValid(State) ? State->GetPlayerStatus() : EPlayerStatus::Idle;
}

//-----------------------------------------------------
//しゃがみ中か
//-----------------------------------------------------
bool APlayerCharacter::IsCrouch() const
{
	return IsValid(m_stateMachine) && m_stateMachine->IsCurrentState<UPlayerCrouchState>();
}

//-----------------------------------------------------
//影状態か
//-----------------------------------------------------
bool APlayerCharacter::IsShadow() const
{
	return IsValid(m_stateMachine) && m_stateMachine->IsCurrentState<UPlayerShadowState>();
}

//-----------------------------------------------------
//ダッシュ中か
//-----------------------------------------------------
bool APlayerCharacter::IsDash() const
{
	return IsValid(m_stateMachine) && m_stateMachine->IsCurrentState<UPlayerDashState>();
}

//-----------------------------------------------------
//ステートマシン取得
//-----------------------------------------------------
UPlayerStateMachine* APlayerCharacter::GetPlayerStateMachine() const
{
	return IsValid(m_stateMachine) ? m_stateMachine.Get() : nullptr;
}

//-----------------------------------------------------
// 上が壁か
//-----------------------------------------------------
bool APlayerCharacter::IsUpPosWall(const FVector& _startPos)const
{
	if (!GetWorld() || !IsValid(m_Capsule)) { return true; }

	//サイズの半分
	float helfHeight = m_Capsule->GetScaledCapsuleHalfHeight();
	//現在が初期値の何分の1か
	float heightScale = m_capsuleHeight / (helfHeight * 2.f);
	float maxCapuselHeightHelf = m_capsuleHeight ;

	//スタート位置
	FVector Start = _startPos + FVector{ 0,0,maxCapuselHeightHelf };

	//通常時のサイズと一緒の高さまでトレース
	FVector End = Start;
	
	//カプセルに変換
	FCollisionShape Capsule = FCollisionShape::MakeCapsule(m_capsuleRadius , maxCapuselHeightHelf);
	

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult HitResult;

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Capsule,
		Params
	);

	return bHit;
}

//-----------------------------------------------------
//影状態か
//-----------------------------------------------------
bool APlayerCharacter::IsInShadow()const
{
	return IsShadow();
}

//-------------------------------------------------------
//足元座標取得
//-------------------------------------------------------
FVector APlayerCharacter::GetFeetLocation() const
{
	//影の上(足元が光に照らされていなければ影状態に
	FVector targetPos = GetActorLocation();
	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	const float capselHeighiHalf = IsValid(Capsule) ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;
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
	if (!CanReceivePlayerInput()) { return; }
	m_charaMoveInput = Value.Get<FVector2D>();
	if (FMath::Abs(m_charaMoveInput.X) < 0.1f) { m_charaMoveInput.X = 0.f; }
	if (FMath::Abs(m_charaMoveInput.Y) < 0.1f) { m_charaMoveInput.Y = 0.f; }
	m_stateMachine->MoveInput();
}

void APlayerCharacter::Enhanced_EndMove(const FInputActionValue& Value)
{
	m_charaMoveInput = FVector2D::ZeroVector;
	if (IsValid(m_stateMachine)) { m_stateMachine->RefreshLocomotionState(); }
}

//------------------------------------------------------
//ダッシュ入力開始処理
//------------------------------------------------------
void APlayerCharacter::Enhanced_OnInputDash(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->SetDashRequested(true); }
}

//------------------------------------------------------
//ダッシュ入力終了処理
//------------------------------------------------------
void APlayerCharacter::Enhanced_EndInputDash(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->SetDashRequested(false); }
}

//------------------------------------------------------
//しゃがみ
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveCrouch(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->ToggleCrouch(); }
}

//------------------------------------------------------
//ジャンプ
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && IsValid(m_stateMachine)) { m_stateMachine->TryJump(); }
}

//------------------------------------------------------
//攻撃
//------------------------------------------------------
void APlayerCharacter::Enhanced_Attack(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->TryAttack(); }
}

//------------------------------------------------------
//影潜り
//------------------------------------------------------
void APlayerCharacter::Enhanced_InShadow(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->ToggleShadow(); }
}

//------------------------------------------------------
//インタラクト
//------------------------------------------------------
void APlayerCharacter::Enhanced_Interact(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->TryInteract(); }
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
	if (!IsValid(Controller)) { return; }

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
		//UE_LOG(LogTemp, Display, TEXT("trueeeeeeeeeeeeeeeeeeeeeeeeeee"));
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
	//UE_LOG(LogTemp, Display, TEXT("unnti"));

	if (!IsValid(OtherActor) || !IsValid(OtherComp)) { return; }
	//影オブジェクトと衝突したか
	if (OtherComp->ComponentHasTag(TEXT("Shadow")) == true)
	{
		//衝突対象を追加
		m_hitActors.Add(OtherActor);
		m_bOnShadow = true;
	}

	//インタラクト可能オブジェクトに触れた時
	if (OtherActor->ActorHasTag(TEXT("Interact")))
	{
		m_bHitIntteractObject = true;
		m_hitInteractOb = Cast<AInteract>(OtherActor);
		m_bHitIntteractObject = IsValid(m_hitInteractOb);
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
	//UE_LOG(LogTemp, Display, TEXT("end Hit"));
	if (!IsValid(OtherActor) || !IsValid(OtherComp)) { return; }

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

//影状態解除

//----------------------------------------------------------
//鍵取得時の関数
//----------------------------------------------------------
void APlayerCharacter::OnGetKeyItem()
{
	AMyPlayerController* PC = Cast<AMyPlayerController>(Controller);
	if (!PC) { return; }
	if (UHUDWidget* HUD = PC->GetHUDWidget())
	{
		if (IsValid(HUD)) { HUD->SetHasKeyItem(true); }
	}

}

//ダッシュ終了処理

//----------------------------------------------------------
//カメラ壁回避け処理
//----------------------------------------------------------
void APlayerCharacter::UpdateCamerawallSiseOffset(float _deltaTime)
{
	if (!GetWorld() || !IsValid(m_pSpringArm)) { return; }

	// スプリングアームの根元（Pivot）
	const FVector Pivot = m_pSpringArm->GetComponentLocation();

	// 「理想の後方位置」（横オフセット無し）
	const FVector BackDir = -m_pSpringArm->GetForwardVector();
	const FVector DesiredBase = Pivot + BackDir * m_pSpringArm->TargetArmLength;

	// 左右に逃がした位置
	const FVector Right = m_pSpringArm->GetRightVector();
	const FVector DesiredR = DesiredBase + Right * WallAvoidSideOffset;
	const FVector DesiredL = DesiredBase - Right * WallAvoidSideOffset;

	auto TraceTo = [&](const FVector& To) -> float
	{
		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(CameraWallAvoid), false, this);
		// 自分に当たらないように（必要ならMeshもIgnore）
		Params.AddIgnoredActor(this);

		const bool bHit = GetWorld()->SweepSingleByChannel(
			Hit,
			Pivot,
			To,
			FQuat::Identity,
			WallAvoidChannel,
			FCollisionShape::MakeSphere(WallAvoidTraceRadius),
			Params
		);

		// “到達できる距離”を返す（遠いほど良い）
		return bHit ? Hit.Distance : FVector::Distance(Pivot, To);
	};

	const float DistR = TraceTo(DesiredR);
	const float DistL = TraceTo(DesiredL);

	// より遠くまで行ける＝壁に当たりにくい方へ
	float TargetY = 0.f;

	const float IdealDist = FVector::Distance(Pivot, DesiredBase);
	const bool bWallClose = (FMath::Min(DistR, DistL) < IdealDist - 1.f); // 壁が近い判定

	if (bWallClose)
		TargetY = (DistR >= DistL) ? +WallAvoidSideOffset : -WallAvoidSideOffset;

	// 滑らかに
	CurrentSideOffsetY = FMath::FInterpTo(CurrentSideOffsetY, TargetY, _deltaTime, WallAvoidInterpSpeed);

	FVector Offset = m_pSpringArm->SocketOffset;
	Offset.Y = CurrentSideOffsetY;
	m_pSpringArm->SocketOffset = Offset;
}
bool APlayerCharacter::CanReceivePlayerInput() const
{
	return IsValid(m_stateMachine) && m_stateMachine->CanControl();
}

bool APlayerCharacter::CanAttack() const
{
	return IsValid(m_stateMachine) && m_stateMachine->IsAttacking();
}

bool APlayerCharacter::IsInvincible() const
{
	return IsValid(m_stateMachine) && m_stateMachine->IsInvincible();
}

float APlayerCharacter::GetShadowTimer() const
{
	return IsValid(m_stateMachine) ? m_stateMachine->GetStaminaUsed() : 0.f;
}

float APlayerCharacter::GetMaxShadowTime() const
{
	return IsValid(m_stateMachine) ? m_stateMachine->GetMaximumStamina() : 0.f;
}

AEnemyBase* APlayerCharacter::GetNearestEnemy()
{
	const UPlayerAttackState* Attack = IsValid(m_stateMachine)
		? m_stateMachine->GetState<UPlayerAttackState>() : nullptr;
	return Attack ? Attack->FindTarget() : nullptr;
}

void APlayerCharacter::OnInteractAnimationEnd()
{
	if (IsValid(m_stateMachine)) { m_stateMachine->NotifyInteractionFinished(); }
}

const FCameraViewSetting& APlayerCharacter::GetCameraViewSetting() const
{
	if (m_cameraStatus == ECameraStatus::SneakKill) { return EventCamera; }
	const UPlayerStateBase* State = IsValid(m_stateMachine)
		? Cast<UPlayerStateBase>(m_stateMachine->GetCurrentState()) : nullptr;
	if (State && State->UsesCameraOverride()) { return State->GetCameraSettings(); }
	return m_cameraStatus == ECameraStatus::TopDownView ? TopDownCamera : ThirdPersonCamera;
}
