//’S“–“úîà
#include "PlayerCharacter.h"
#include "State/Player/PlayerCrouchState.h"
#include "State/Player/PlayerShadowState.h"
#include "State/Player/PlayerDashState.h"
//----------------------------------------------------------
// ŠT—v				FƒvƒŒƒCƒ„[ƒLƒƒƒ‰‚ğ§Œä‚·‚éCharacterƒIƒuƒWƒFƒNƒg
// XV“ú@@@@ @F11/19@
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
#include "Engine/OverlapResult.h"
#include "CollisionShape.h"
#include "Engine/EngineTypes.h"
#include "Engine/CollisionProfile.h"
#include "GameInstance/PlayDataInfo.h"
#include "GameInstance/PlayDataGameInstanceSubsystem.h"
#include "Checkpoint/CheckpointManager.h"
#include "Interact/Interact.h"
#include "Camera/CameraFocusDirectorComponent.h"
#include "UI/HUD/HUDWidget.h"
#include "StealthAction/PlayerCharacter/Controller/MyPlayerController.h"


//----------------------------------------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
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
	//–ˆƒtƒŒ[ƒ€Tick‚ğŒÄ‚Ô‚©Œˆ‚ß‚éƒtƒ‰ƒO
	PrimaryActorTick.bCanEverTick = true;

	//ƒJƒƒ‰‰ñ“]‚ÉƒLƒƒƒ‰‚ğ’¼Ú’Ç]‚³‚¹‚È‚¢
	bUseControllerRotationYaw = false;

	//©“®‰ñ“]‚ğs‚í‚È‚¢
	GetCharacterMovement()->bOrientRotationToMovement = false;

	//ƒfƒtƒHƒ‹ƒgƒvƒŒƒCƒ„[‚Æ‚µ‚Äİ’è
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//“®“I”z—ñTArray‚Ì‰Šú‰»
	m_pHitActors.Reset();

	// Shared camera modes; state-specific camera profiles live on each state BP.
	ThirdPersonCamera = { FRotator(0.f, 0.f, 0.f), 70.f, 400.f, FVector(0.f, 70.f, 20.f), 1.f };
	TopDownCamera = { FRotator(0.f, 0.f, 0.f), 90.f, 1000.f, FVector(0.f, 70.f, 20.f), 5.f };
	EventCamera = ThirdPersonCamera;

	//ƒJƒvƒZƒ‹‚Ì‰Šú’l‚ğ‹L˜^
	m_Capsule = GetCapsuleComponent();
	m_capsuleRadius = 40.f;
	m_capsuleHeight = 80.f;

	//ƒXƒvƒŠƒ“ƒOƒA[ƒ€‚ÌƒIƒuƒWƒFƒNƒg‚ğ¶¬
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm != NULL) {
		m_pSpringArm->SetupAttachment(RootComponent);

		m_pSpringArm->TargetArmLength = ThirdPersonCamera.springArmLength;					//ƒJƒƒ‰‹——£
		m_pSpringArm->SocketOffset = FVector(0, m_maxOffsetY, 20.f);	//Œ¨‰z‚µ‹“_İ’è
		m_pSpringArm->bUsePawnControlRotation = true;			//ƒJƒƒ‰‰ñ“]
		m_pSpringArm->bEnableCameraLag = true;
		m_pSpringArm->CameraLagSpeed = 100.f;
		m_pSpringArm->bDoCollisionTest = true;                 // •Ç©“®‰ñ”ğ
	}

	//Œ•‚ÌUŒ‚ƒRƒ“ƒ|[ƒlƒ“ƒg¶¬
	m_sword = CreateDefaultSubobject<USwordAttackComponent>(TEXT("Sword"));
	if (IsValid(m_sword))
	{
		m_sword->SetupAttachment(GetCapsuleComponent());
		m_sword->SetRelativeLocation(FVector::ZeroVector);
		m_sword->SetRelativeRotation(FRotator::ZeroRotator);

	}

	//ƒJƒƒ‰ƒIƒuƒWƒFƒNƒg‚ğİ’è
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if ((m_pCamera != nullptr) && (m_pSpringArm != nullptr)) {
		//ƒJƒƒ‰‚ğƒXƒvƒŠƒ“ƒOƒA[ƒ€‚Éƒ^ƒbƒ`‚³‚¹‚é
		m_pCamera->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);
		m_pCamera->bUsePawnControlRotation = false;

	}

	//ƒJƒƒ‰ƒtƒH[ƒJƒX—pƒRƒ“ƒ|[ƒlƒ“ƒg¶¬
	m_pCameraFocusDirector = CreateDefaultSubobject<UCameraFocusDirectorComponent>(TEXT("CameraFocusDirector"));
}

//----------------------------------------------------------
// BeginPlayiƒQ[ƒ€ŠJnj
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

	//‰ŠúƒƒbƒVƒ…
	if (m_defaultMesh)
	{
		GetMesh()->SetSkeletalMesh(m_defaultMesh);
		m_Capsule->SetCapsuleSize(m_capsuleRadius, m_capsuleHeight);
	}

	m_pSpringArm->SetRelativeRotation(ThirdPersonCamera.rotator);
	m_pCamera->FieldOfView = ThirdPersonCamera.fieldOfView;

	//PlayerCotroller‚©‚çEnhancedInputSubSystem‚ğæ“¾
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		//ƒJƒƒ‰ƒsƒbƒ`‚Ì§ŒÀ
		if (IsValid(PC->PlayerCameraManager))
		{
			PC->PlayerCameraManager->ViewPitchMin = -60.f;
			PC->PlayerCameraManager->ViewPitchMax = 20.f;
		}

	}

	UCapsuleComponent* capsel = GetCapsuleComponent();
	//ƒCƒxƒ“ƒg‚ğ”­¶‚³‚¹‚é‚©true
	capsel->SetGenerateOverlapEvents(true);
	//ƒR[ƒ‹ƒoƒbƒNŠÖ”“o˜^
	//ƒRƒŠƒWƒ‡ƒ“ƒvƒŠƒZƒbƒg‚ğƒvƒŒƒCƒ„[
	capsel->SetCollisionProfileName(TEXT("Player"));

	//ƒR[ƒ‹ƒoƒbƒNŠÖ”‚ğ“o˜^
	capsel->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlap);
	capsel->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlap);

	////ƒJƒƒ‰‚Ìk‚æ‚è
	//m_pSpringArm->TargetArmLength = ThirdPersonCamera.springArmLength;

	//ƒ\[ƒhUŒ‚ƒRƒ“ƒ|[ƒlƒ“ƒg‚Ì‰Šú‰»
	if (IsValid(m_sword))
	{
		//”O‚Ì‚½‚ßÕ“Ë‘ÎÛƒNƒŠƒA
		m_sword->ClearCollisionObjectType();
		//ƒRƒŠƒWƒ‡ƒ“preset–¼‚Ìİ’è
		m_sword->SetProfileName(FName("PlayerWeapon"));
		//U‚èI‚í‚è‚ÌƒR[ƒ‹ƒoƒbƒNŠÖ”“o˜^
		m_sword->RegisterSwingEndCallback(CreateSwingEndCallback(APlayerCharacter::OnAttackEnd));
	}

	//ƒRƒ“ƒeƒjƒ…[‚µ‚Ä‚¢‚ê‚Îî•ñæ“¾
	//ƒvƒŒƒCƒf[ƒ^‚Ìæ“¾
	UPlayDataGameInstanceSubsystem* pPlayData = (GetWorld() && GetWorld()->GetGameInstance() ? GetWorld()->GetGameInstance()->GetSubsystem<UPlayDataGameInstanceSubsystem>() : nullptr);
	UCheckpointManager* pCheckMng = (GetWorld() ? GetWorld()->GetSubsystem<UCheckpointManager>() : nullptr);
	if (pPlayData && pPlayData->IsIsContinued() && pPlayData->GetCheckpointInfo().Index != -1) {
		m_playerInfo = pPlayData->GetPlayerInfo();
		//ƒ`ƒFƒbƒNƒ|ƒCƒ“ƒg‚ğˆê‚Â‚Å‚àG‚ê‚Ä‚¢‚ê‚Î
		if (pCheckMng && pCheckMng->GetCurrentCheckpointIndex() >= 0)
		{
			SetActorLocation(pCheckMng->GetCurrentCheckpointLocation());
		}
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
// Ticki–ˆƒtƒŒ[ƒ€XVj
//----------------------------------------------------------
void APlayerCharacter::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	//ƒJƒƒ‰‚ÌXVˆ—
	UpdateCamera(_deltaTime);
	//ƒJƒƒ‰‚ÆÕ•Á•¨‚ÌƒRƒŠƒWƒ‡ƒ“”»’è
	UpdateCameraCollision();

	// Update only the active state registered by the machine Blueprint.
	if (IsValid(m_stateMachine))
	{
		m_stateMachine->Update(_deltaTime);
	}

	//‹“_•ÏX
	ViewpointSwitching(_deltaTime);

	//Œ»İˆÊ’u‚Å’Êíó‘Ô‚É–ß‚Á‚½‚ç•Ç‚É‚Ì‚ß‚è‚Ş‚©
	if (!IsUpPosWall(GetFeetLocation()))
	{
		m_saveLastNotUpWallPos = GetActorLocation();
	}

	UpdateMeshCameraVisibility();
}

//-----------------------------------------------------
//ƒJƒƒ‰‚ª‹ß‚¢‚ÌƒƒbƒVƒ…•\¦
//-----------------------------------------------------
void APlayerCharacter::UpdateMeshCameraVisibility()
{
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (!IsValid(PlayerMesh))
	{
		return;
	}

	//‰e‚Ìo“ü‚è‚Å‚ÍA’¾‚Şˆ—‚ÆƒJƒƒ‰‚Ì”ñ•\¦‚ğd‚Ë‚È‚¢
	bool bHideForCamera = false;
	if (!IsShadow() && !m_bCameraSwitching)
	{
		const UCameraComponent* Cam = FindComponentByClass<UCameraComponent>();
		if (IsValid(Cam))
		{
			bHideForCamera = FVector::Distance(Cam->GetComponentLocation(), PlayerMesh->GetComponentLocation()) < 150.f;
		}
	}
	PlayerMesh->SetHiddenInGame(bHideForCamera, true);
}

//----------------------------------------------------------
// “ü—ÍƒoƒCƒ“ƒh
//----------------------------------------------------------
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent¨EnhancedInputComponent•ÏŠ·
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInput) { return; }

	//X²
	if (IsValid(m_moveIA)) { EnhancedInput->BindAction(m_moveIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_Move); }
	if (IsValid(m_moveIA)) { EnhancedInput->BindAction(m_moveIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_EndMove); }
	if (IsValid(m_moveIA)) { EnhancedInput->BindAction(m_moveIA, ETriggerEvent::Canceled, this, &APlayerCharacter::Enhanced_EndMove); }
	//‚µ‚á‚ª‚İ
	if (IsValid(m_moveCrouchIA)) { EnhancedInput->BindAction(m_moveCrouchIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_MoveCrouch); }
	//ƒWƒƒƒ“ƒv
	if (IsValid(m_moveJumpIA)) { EnhancedInput->BindAction(m_moveJumpIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_MoveJump); }

	//UŒ‚
	if (IsValid(m_attackIA)) { EnhancedInput->BindAction(m_attackIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_Attack); }
	//•Ší•ÏX
	if (IsValid(m_changeWeaponIA)) { EnhancedInput->BindAction(m_changeWeaponIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_changeWeapon); }
	//‰eö‚è
	if (IsValid(m_inShadowIA)) { EnhancedInput->BindAction(m_inShadowIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_InShadow); }
	//ƒCƒ“ƒ^ƒ‰ƒNƒg
	if (IsValid(m_interactIA)) { EnhancedInput->BindAction(m_interactIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_Interact); }

	//ƒJƒƒ‰Pitch
	if (IsValid(m_cameraPitchIA)) { EnhancedInput->BindAction(m_cameraPitchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraPitch); }
	//ƒJƒƒ‰Yaw
	if (IsValid(m_cameraYawIA)) { EnhancedInput->BindAction(m_cameraYawIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraYaw); }
	//ƒJƒƒ‰Reset
	if (IsValid(m_cameraResetIA)) { EnhancedInput->BindAction(m_cameraResetIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraReset); }
	//‹“_•ÏX
	if (IsValid(m_cameraSwitchIA)) { EnhancedInput->BindAction(m_cameraSwitchIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Enhanced_CameraSwitch); }

	//ƒ_ƒbƒVƒ…
	if (IsValid(m_moveDashIA)) { EnhancedInput->BindAction(m_moveDashIA, ETriggerEvent::Started, this, &APlayerCharacter::Enhanced_OnInputDash); }
	//ƒ_ƒbƒVƒ…
	if (IsValid(m_moveDashIA)) { EnhancedInput->BindAction(m_moveDashIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_EndInputDash); }

	//ƒJƒƒ‰Pitch
	if (IsValid(m_cameraPitchIA)) { EnhancedInput->BindAction(m_cameraPitchIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_CameraPitchReleased); }
	//ƒJƒƒ‰Yaw
	if (IsValid(m_cameraYawIA)) { EnhancedInput->BindAction(m_cameraYawIA, ETriggerEvent::Completed, this, &APlayerCharacter::Enhanced_CameraYawReleased); }
}

//----------------------------------------------------------
// ƒJƒƒ‰XV
//----------------------------------------------------------
void APlayerCharacter::UpdateCamera(float _deltaTime)
{
	if (!IsValid(m_pSpringArm) || !IsValid(GetCharacterMovement())) { return; }

	//ƒJƒƒ‰‚ÌƒXƒe[ƒ^ƒX‚ª˜ëáÕ‚È‚çˆ—‚µ‚È‚¢
	if (m_cameraStatus == ECameraStatus::TopDownView) { return; }
	if (IsValid(m_stateMachine) && m_stateMachine->IsCurrentState<UPlayerAttackState>()) { return; }
	//if (m_bCameraSwitching) { return; }

	//ˆ——‚¿‚µ‚Ä‚àˆê’è‘¬“x‚ÅƒJƒƒ‰‚ª‰ñ‚é‚æ‚¤‚É•â³
	float rotateCorrection = CGameUtility::GetFpsCorrection(_deltaTime);

	AddControllerYawInput(m_cameraRotateInput.X);
	AddControllerPitchInput(m_cameraRotateInput.Y);

	//‰eö‚è’†‚ÍƒJƒƒ‰ƒIƒtƒZƒbƒg‚ğ•ÏX‚µ‚È‚¢
	const UPlayerStateBase* CameraState = IsValid(m_stateMachine)
		? Cast<UPlayerStateBase>(m_stateMachine->GetCurrentState()) : nullptr;
	if (CameraState && CameraState->UsesCameraOverride()) { return; }

	//X²“ü—Í‚ª‚ ‚èAˆÚ“®“ü—Í‚ª‚È‚¯‚ê‚Î
	//ƒJƒƒ‰ƒIƒtƒZƒbƒg‚ğˆÚ“®‚³‚¹A¶‰E‚Ç‚¿‚ç‚ÌŒ¨‰z‚µ‹“_‚É‘Î‰‚Å‚«‚é‚æ‚¤‚É‚µ
	//Šp‚Ìæ‚ğŒ©‚â‚·‚­‚·‚é
	if (m_cameraRotateInput.X != 0 && GetCharacterMovement()->Velocity.Length() <= GetCharacterMovement()->MaxWalkSpeed) {
		float moveOffsetDir = (m_cameraRotateInput.X > 0) ? -1.f : 1.f;

		//Œ»İ‚ÌƒIƒtƒZƒbƒgˆÊ’u
		float targetOffset = m_pSpringArm->SocketOffset.Y;
		//100ƒXƒs[ƒh
		targetOffset += moveOffsetDir * 70.f * _deltaTime;

		//Å‘å’lƒ`ƒFƒbƒN
		if (targetOffset > m_maxOffsetY)
		{
			targetOffset = m_maxOffsetY;
		}
		else if (targetOffset < -m_maxOffsetY)
		{
			targetOffset = -m_maxOffsetY;
		}

		m_pSpringArm->SocketOffset.Y = targetOffset;

		//“ü—Í‚ª‚È‚¢ŠÔŒv‘ª—pƒ^ƒCƒ}[ƒŠƒZƒbƒg
		m_OutcameraInputTimer = 0.f;
	}
	
	//‰ŠúƒIƒtƒZƒbƒgˆÊ’u‚É–ß‚·ˆ—
	else
	{
		//ƒ^ƒCƒ}[‰ÁZ
		m_OutcameraInputTimer += _deltaTime;
		//‚±‚Ìƒ}ƒWƒbƒNƒiƒ“ƒo[‚æ‚èŠÔ‚½‚Á‚Ä‚¢‚ê‚Îˆ—‚·‚é
		if (m_OutcameraInputTimer > 0.3f)
		{

			//Œ»İ‚ÌƒIƒtƒZƒbƒgˆÊ’u
			float socketOffsetPosY = m_pSpringArm->SocketOffset.Y;

			//Œ»İˆÊ’u‚Æ‰ŠúƒIƒtƒZƒbƒg‚Ì·
			float socketLeght = socketOffsetPosY - m_initOffsetY;

			//‰ŠúˆÊ’u‚©‚ç‚¸‚ê‚Ä‚¢‚éê‡‚Í–ß‚·
			if (!FMath::IsNearlyZero(abs(socketLeght)))
			{
				//’†‰›+-‚ª‚Ç‚Á‚¿‚©
				float moveOffsetDir = (socketOffsetPosY > m_initOffsetY) ? -1.f : 1.f;

				//ƒ\ƒPƒbƒgˆÊ’uˆÚ“®
				socketOffsetPosY += moveOffsetDir * 2.f * _deltaTime;
				m_pSpringArm->SocketOffset.Y = socketOffsetPosY;

				//‚ ‚é’ö“x‹ß‚Ã‚¢‚½‚ç‡‚í‚¹‚é
				if (abs(socketLeght) < 0.5f)
				{
					m_pSpringArm->SocketOffset.Y = m_initOffsetY;
				}

			}
		}
	}
}

//----------------------------------------------------------
//èU‚ê•â³ˆ—
//----------------------------------------------------------
void APlayerCharacter::UpdateCameraShake(float _deltaTime)
{
	if (!IsValid(m_pSpringArm)) { return; }

	//Œ»İ‚Ìƒ\ƒPƒbƒgƒIƒtƒZƒbƒg
	FVector currentSocketOffset = m_pSpringArm->SocketOffset;
}

//----------------------------------------------------------
// ƒJƒƒ‰Õ“Ë”»’èˆ—
//----------------------------------------------------------
void APlayerCharacter::UpdateCameraCollision()
{
	//ƒJƒƒ‰Õ“Ëˆ—‚ÌŠg’£—p

}

//ƒXƒ^ƒ~ƒiÁ”ï

//ƒXƒ^ƒ~ƒi‰ñ•œ

//----------------------------------------------------------
// ‹“_Ø‚è‘Ö‚¦
//----------------------------------------------------------
void APlayerCharacter::ViewpointSwitching(float _deltaTime)
{
	const FCameraViewSetting& View = GetCameraViewSetting();
	if (!IsValid(m_pSpringArm) || !IsValid(m_pCamera)) { return; }

	//‹“_•ÏŠ·’†‚Å‚È‚¯‚ê‚Îˆ—‚µ‚È‚¢
	if (!m_bCameraSwitching) {
		return;
	}
	//

	// ‚Ü‚¸‚Í‹——£
	//ˆø”‚Í@Œ»İ’nA–Ú•W’lAƒfƒ‹ƒ^ƒ^ƒCƒ€A•âŠÔƒXƒs[ƒh
	float NewLength = FMath::FInterpTo(
		m_pSpringArm->TargetArmLength,
		View.springArmLength,
		_deltaTime,
		View.InterpSpeed
	);
	m_pSpringArm->TargetArmLength = NewLength;
	m_pSpringArm->SocketOffset = FMath::VInterpTo(m_pSpringArm->SocketOffset, View.ofSet, _deltaTime, View.InterpSpeed);

	//‹–ìŠp
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
		//‰ñ“]
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
		
		//–Ú“I‰ñ“]‚É‹ß‚Ã‚¢‚½‚ç‡‚í‚¹‚é
		if (abs(newRot.Yaw - View.rotator.Yaw) < 1.f)
		{
			m_pSpringArm->SetRelativeRotation(View.rotator);
		}
	}

	//ˆê’è‹——£ˆÈ‰º‚É‚È‚ê‚Î–Ú•W’n“_‚Æ“¯‚¶À•W‚É‚·‚é
	//–Ú•W’n“_‚Æ‚Ì·
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
		//‹“_Ø‚è‘Ö‚¦’†ƒtƒ‰ƒO‚ğfalse‚É
		m_bCameraSwitching = false;

	}
}

//-----------------------------------------------------
//UŒ‚I—¹ˆ—
//-----------------------------------------------------
void APlayerCharacter::OnAttackEnd()
{
	if (IsValid(m_stateMachine)) { m_stateMachine->NotifyAttackFinished(); }
}

//-----------------------------------------------------
//ƒJƒƒ‰ƒtƒH[ƒJƒXˆ—
//-----------------------------------------------------
void APlayerCharacter::StartCameraFocus(AActor* const _cameraActor, float _blendTime)
{
	if (!IsValid(_cameraActor) || !IsValid(m_pCameraFocusDirector)) { return; }

	if (!IsValid(Controller)) { return; }
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC) { return; }

	//ƒJƒƒ‰‚ÌØ‚è‘Ö‚¦‘O‚ÌƒAƒNƒ^[‚ğ•Û‘¶
	m_pSaveCameraActor = PC->GetViewTarget();

	//ƒJƒƒ‰‚ÌØ‚è‘Ö‚¦
	const bool bCouldControl = m_bCanControl;
	bool isStat = false;
	m_pCameraFocusDirector->StartFocusByProviderActor(_cameraActor, isStat, PC);
	if (!isStat) { m_bCanControl = bCouldControl; return; }
	m_bCanControl = false;

	if (IsValid(m_stateMachine)) { m_stateMachine->CancelDash(); m_stateMachine->RefreshLocomotionState(); }

}

//------------------------------------------------------
//ƒJƒƒ‰ƒtƒH[ƒJƒXI—¹ˆ—
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
// ƒWƒƒƒ“ƒvI—¹ˆ—
//------------------------------------------------------
void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (IsValid(m_stateMachine)) { m_stateMachine->NotifyLanded(); }
}

//------------------------------------------------------
//ƒCƒxƒ“ƒgó‘Ô‚ÌƒJƒƒ‰ƒtƒH[ƒJƒXI—¹
//------------------------------------------------------
void APlayerCharacter::SetEventCameraChange(FCameraViewSetting _viewSetting)
{
	EventCamera = _viewSetting;
	m_bCameraSwitching = true;
	m_cameraStatus = ECameraStatus::SneakKill;
}

//-----------------------------------------------------
//ƒ_ƒ[ƒWˆ—
//-----------------------------------------------------
void APlayerCharacter::OnDamage(int32 Damage, FVector KnockBackVec, bool bSneakKill)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->ReceiveDamage(Damage, KnockBackVec); }
}

//-----------------------------------------------------
//ó‘Ô•ÏXˆ—
//-----------------------------------------------------
//-----------------------------------------------------
//Œ»İƒXƒe[ƒgæ“¾
//-----------------------------------------------------
UPlayerStateBase* APlayerCharacter::GetPlayerState() const
{
	return IsValid(m_stateMachine) ? Cast<UPlayerStateBase>(m_stateMachine->GetCurrentState()) : nullptr;
}

//-----------------------------------------------------
//Œ»İƒXƒe[ƒg‚ÌAnimBP—p‚Ìó‘Ôæ“¾
//-----------------------------------------------------
EPlayerStatus APlayerCharacter::GetPlayerStatus() const
{
	const UPlayerStateBase* State = GetPlayerState();
	return IsValid(State) ? State->GetPlayerStatus() : EPlayerStatus::Idle;
}

//-----------------------------------------------------
//‚µ‚á‚ª‚İ’†‚©
//-----------------------------------------------------
bool APlayerCharacter::IsCrouch() const
{
	return IsValid(m_stateMachine) && m_stateMachine->IsCurrentState<UPlayerCrouchState>();
}

//-----------------------------------------------------
//‰eó‘Ô‚©
//-----------------------------------------------------
bool APlayerCharacter::IsShadow() const
{
	return IsValid(m_stateMachine) && m_stateMachine->IsCurrentState<UPlayerShadowState>();
}

//-----------------------------------------------------
//ƒ_ƒbƒVƒ…’†‚©
//-----------------------------------------------------
bool APlayerCharacter::IsDash() const
{
	return IsValid(m_stateMachine) && m_stateMachine->IsCurrentState<UPlayerDashState>();
}

//-----------------------------------------------------
//ƒXƒe[ƒgƒ}ƒVƒ“æ“¾
//-----------------------------------------------------
UPlayerStateMachine* APlayerCharacter::GetPlayerStateMachine() const
{
	return IsValid(m_stateMachine) ? m_stateMachine.Get() : nullptr;
}

//-----------------------------------------------------
// ã‚ª•Ç‚©
//-----------------------------------------------------
bool APlayerCharacter::IsUpPosWall(const FVector& _startPos)const
{
	if (!GetWorld() || !IsValid(m_Capsule)) { return true; }

	//ƒTƒCƒY‚Ì”¼•ª
	float helfHeight = m_Capsule->GetScaledCapsuleHalfHeight();
	//Œ»İ‚ª‰Šú’l‚Ì‰½•ª‚Ì1‚©
	float heightScale = m_capsuleHeight / (helfHeight * 2.f);
	float maxCapuselHeightHelf = m_capsuleHeight ;

	//ƒXƒ^[ƒgˆÊ’u
	FVector Start = _startPos + FVector{ 0,0,maxCapuselHeightHelf };

	//’Êí‚ÌƒTƒCƒY‚Æˆê‚Ì‚‚³‚Ü‚ÅƒgƒŒ[ƒX
	FVector End = Start;
	
	//ƒJƒvƒZƒ‹‚É•ÏŠ·
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
//‰eó‘Ô‚©
//-----------------------------------------------------
bool APlayerCharacter::IsInShadow()const
{
	return IsShadow();
}

//-------------------------------------------------------
//‘«Œ³À•Wæ“¾
//-------------------------------------------------------
FVector APlayerCharacter::GetFeetLocation() const
{
	//‰e‚Ìã(‘«Œ³‚ªŒõ‚ÉÆ‚ç‚³‚ê‚Ä‚¢‚È‚¯‚ê‚Î‰eó‘Ô‚É
	FVector targetPos = GetActorLocation();
	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	const float capselHeighiHalf = IsValid(Capsule) ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;
	targetPos.Z -= capselHeighiHalf;
	return targetPos;
}

//------------------------------------------------------
//ƒvƒŒƒCƒ„[î•ñæ“¾
//------------------------------------------------------
FPlayerInfo APlayerCharacter::GetPlayerInfo()
{
	return m_playerInfo;
}

//------------------------------------------------------
//ˆÚ“®
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
//ƒ_ƒbƒVƒ…“ü—ÍŠJnˆ—
//------------------------------------------------------
void APlayerCharacter::Enhanced_OnInputDash(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->SetDashRequested(true); }
}

//------------------------------------------------------
//ƒ_ƒbƒVƒ…“ü—ÍI—¹ˆ—
//------------------------------------------------------
void APlayerCharacter::Enhanced_EndInputDash(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->SetDashRequested(false); }
}

//------------------------------------------------------
//‚µ‚á‚ª‚İ
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveCrouch(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->ToggleCrouch(); }
}

//------------------------------------------------------
//ƒWƒƒƒ“ƒv
//------------------------------------------------------
void APlayerCharacter::Enhanced_MoveJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && IsValid(m_stateMachine)) { m_stateMachine->TryJump(); }
}

//------------------------------------------------------
//UŒ‚
//------------------------------------------------------
void APlayerCharacter::Enhanced_Attack(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->TryAttack(); }
}

//------------------------------------------------------
//‰eö‚è
//------------------------------------------------------
void APlayerCharacter::Enhanced_InShadow(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->ToggleShadow(); }
}

//------------------------------------------------------
//ƒCƒ“ƒ^ƒ‰ƒNƒg
//------------------------------------------------------
void APlayerCharacter::Enhanced_Interact(const FInputActionValue& Value)
{
	if (IsValid(m_stateMachine)) { m_stateMachine->TryInteract(); }
}

//------------------------------------------------------
//•Ší•ÏX
//------------------------------------------------------
void APlayerCharacter::Enhanced_changeWeapon(const FInputActionValue& Value)
{

}

//------------------------------------------------------
//ƒJƒƒ‰Pitch
//------------------------------------------------------
void APlayerCharacter::Enhanced_CameraPitch(const FInputActionValue& Value)
{
	m_cameraRotateInput.Y = -Value.Get<float>();

	//ƒfƒbƒhƒ][ƒ“
	if (abs(m_cameraRotateInput.Y) < 0.1f)
	{
		m_cameraRotateInput.Y = 0.f;
	}
}

//------------------------------------------------------
//ƒJƒƒ‰Yaw
//------------------------------------------------------
void APlayerCharacter::Enhanced_CameraYaw(const FInputActionValue& Value)
{
	m_cameraRotateInput.X = Value.Get<float>();

	//ƒfƒbƒhƒ][ƒ“
	if (abs(m_cameraRotateInput.X) < 0.1f)
	{
		m_cameraRotateInput.X = 0.f;
	}
}

//------------------------------------------------------
//ƒJƒƒ‰Reset
//------------------------------------------------------
void APlayerCharacter::Enhanced_CameraReset(const FInputActionValue& Value)
{
	if (!IsValid(Controller)) { return; }

	//“ü—Í‚ª‚È‚¯‚ê‚Î‰½‚à‚µ‚È‚¢
	if (!Value.Get<bool>()) { return; }

	//ƒvƒŒƒCƒ„[‚ÌŒü‚«‚ğæ“¾
	FRotator targetRot = GetActorRotation();

	//Yaw‚ğ‡‚í‚¹‚é
	FRotator NewControlRot = Controller->GetControlRotation();
	NewControlRot.Yaw = targetRot.Yaw;

	Controller->SetControlRotation(NewControlRot);
}

//----------------------------------------------------------
//ƒJƒƒ‰‹“_•ÏX
//----------------------------------------------------------
void APlayerCharacter::Enhanced_CameraSwitch(const FInputActionValue& Value)
{
	//‹“_•ÏX’†ƒtƒ‰ƒOON(true‚µ‚©‚©‚¦‚Á‚Ä—ˆ‚È‚¢—lİ’è‚µ‚Ä‚é
	m_bCameraSwitching = Value.Get<bool>();
	m_cameraStatus = (m_cameraStatus == ECameraStatus::ThirdPerson) ? ECameraStatus::TopDownView : ECameraStatus::ThirdPerson;

	if (m_bCameraSwitching) {
	}
}

//-----------------------------------------------------------
//—£‚³‚ê‚½uŠÔ’l‚ğ‚O‚É‚·‚é‚æ‚¤
//ƒJƒƒ‰ƒsƒbƒ`
//-------------------------------------------------------------
void APlayerCharacter::Enhanced_CameraPitchReleased(const FInputActionValue& Value)
{
	m_cameraRotateInput.Y = 0.f;
}

//-----------------------------------------------------------
//—£‚³‚ê‚½uŠÔ’l‚ğ‚O‚É‚·‚é‚æ‚¤
//ƒJƒƒ‰ƒˆ[
//-------------------------------------------------------------
void APlayerCharacter::Enhanced_CameraYawReleased(const FInputActionValue& Value)
{
	m_cameraRotateInput.X = 0.f;
}

//-----------------------------------------------
//Õ“Ë‚ÌƒR[ƒ‹ƒoƒbƒNŠÖ”
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

	if (!IsValid(OtherActor) || !IsValid(OtherComp)) { return; }
	//‰eƒIƒuƒWƒFƒNƒg‚ÆÕ“Ë‚µ‚½‚©
	if (OtherComp->ComponentHasTag(TEXT("Shadow")) == true)
	{
		//Õ“Ë‘ÎÛ‚ğ’Ç‰Á
		m_hitActors.Add(OtherActor);
		m_bOnShadow = true;
	}

	//ƒCƒ“ƒ^ƒ‰ƒNƒg‰Â”\ƒIƒuƒWƒFƒNƒg‚ÉG‚ê‚½
	if (OtherActor->ActorHasTag(TEXT("Interact")))
	{
		RefreshInteractTarget();
	}

}

//-------------------------------------------------
//Õ“ËI—¹‚ÌƒR[ƒ‹ƒoƒbƒNŠÖ”
//-------------------------------------------------
void APlayerCharacter::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (!IsValid(OtherActor) || !IsValid(OtherComp)) { return; }

	if (OtherComp->ComponentHasTag(TEXT("Shadow")))
	{
		//Õ“Ë‘ÎÛ‚ğ’Ç‰Á
		m_hitActors.Remove(OtherActor);
		if (m_hitActors.Num() <= 0)
			m_bOnShadow = false;
	}

	//ƒCƒ“ƒ^ƒ‰ƒNƒg‰Â”\ƒIƒuƒWƒFƒNƒg‚ÉG‚ê‚½
	if (OtherActor->ActorHasTag(TEXT("Interact")))
	{
		//•Ê‚Ì‘ÎÛ‚â•ÊƒRƒ“ƒ|[ƒlƒ“ƒg‚É‚Ü‚¾G‚ê‚Ä‚¢‚éê‡‚à’²‚×’¼‚·
		RefreshInteractTarget();
	}
}

//ƒfƒoƒbƒN—p
void APlayerCharacter::OnNoiseHeard(const int& _noiseVolume, const FVector& _pos)
{
	m_bOnShadow = false;
}

//‰eó‘Ô‰ğœ

//----------------------------------------------------------
//Œ®æ“¾‚ÌŠÖ”
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

//ƒ_ƒbƒVƒ…I—¹ˆ—

//----------------------------------------------------------
//ƒJƒƒ‰•Ç‰ñ”ğ‚¯ˆ—
//----------------------------------------------------------
void APlayerCharacter::UpdateCamerawallSiseOffset(float _deltaTime)
{
	if (!GetWorld() || !IsValid(m_pSpringArm)) { return; }

	// ƒXƒvƒŠƒ“ƒOƒA[ƒ€‚ÌªŒ³iPivotj
	const FVector Pivot = m_pSpringArm->GetComponentLocation();

	// u—‘z‚ÌŒã•ûˆÊ’uvi‰¡ƒIƒtƒZƒbƒg–³‚µj
	const FVector BackDir = -m_pSpringArm->GetForwardVector();
	const FVector DesiredBase = Pivot + BackDir * m_pSpringArm->TargetArmLength;

	// ¶‰E‚É“¦‚ª‚µ‚½ˆÊ’u
	const FVector Right = m_pSpringArm->GetRightVector();
	const FVector DesiredR = DesiredBase + Right * WallAvoidSideOffset;
	const FVector DesiredL = DesiredBase - Right * WallAvoidSideOffset;

	auto TraceTo = [&](const FVector& To) -> float
	{
		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(CameraWallAvoid), false, this);
		// ©•ª‚É“–‚½‚ç‚È‚¢‚æ‚¤‚Éi•K—v‚È‚çMesh‚àIgnorej
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

		// g“’B‚Å‚«‚é‹——£h‚ğ•Ô‚·i‰“‚¢‚Ù‚Ç—Ç‚¢j
		return bHit ? Hit.Distance : FVector::Distance(Pivot, To);
	};

	const float DistR = TraceTo(DesiredR);
	const float DistL = TraceTo(DesiredL);

	// ‚æ‚è‰“‚­‚Ü‚Ås‚¯‚é•Ç‚É“–‚½‚è‚É‚­‚¢•û‚Ö
	float TargetY = 0.f;

	const float IdealDist = FVector::Distance(Pivot, DesiredBase);
	const bool bWallClose = (FMath::Min(DistR, DistL) < IdealDist - 1.f); // •Ç‚ª‹ß‚¢”»’è

	if (bWallClose)
		TargetY = (DistR >= DistL) ? +WallAvoidSideOffset : -WallAvoidSideOffset;

	// ŠŠ‚ç‚©‚É
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

//-----------------------------------------------------
//Œ»İ‚ÌƒJƒvƒZƒ‹‚Æd‚È‚Á‚Ä‚¢‚éƒCƒ“ƒ^ƒ‰ƒNƒg‘ÎÛ‚ğæ“¾
//-----------------------------------------------------
void APlayerCharacter::GetNearbyInteractTargets(TArray<AInteract*>& Targets) const
{
	Targets.Reset();
	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	UWorld* World = GetWorld();
	if (!World || !IsValid(Capsule) || !Capsule->IsQueryCollisionEnabled()) { return; }
	FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_GameTraceChannel6); //Interact
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(Overlaps, Capsule->GetComponentLocation(), Capsule->GetComponentQuat(),
		Objects, Capsule->GetCollisionShape(), Params);
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AInteract* Target = Cast<AInteract>(Overlap.GetActor());
		if (IsValid(Target) && Target->CanInteract()) { Targets.AddUnique(Target); }
	}
}

//-----------------------------------------------------
//Œ»İ‚ÌÚG‚©‚çˆê”Ô‹ß‚¢‘ÎÛ‚ğæ“¾
//-----------------------------------------------------
AInteract* APlayerCharacter::FindInteractTarget() const
{
	TArray<AInteract*> Targets;
	GetNearbyInteractTargets(Targets);
	AInteract* Nearest = nullptr;
	double BestDistance = 0.0;
	for (AInteract* Target : Targets)
	{
		const double Distance = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());
		if (!Nearest || Distance < BestDistance) { Nearest = Target; BestDistance = Distance; }
	}
	return Nearest;
}

//-----------------------------------------------------
//‘€ì’†‚Ì‘ÎÛ‚ª”ÍˆÍ“à‚Éc‚Á‚Ä‚¢‚é‚©
//-----------------------------------------------------
bool APlayerCharacter::IsInInteractRange(const AInteract* Target) const
{
	if (!IsValid(Target) || !Target->CanInteract()) { return false; }
	TArray<AInteract*> Targets;
	GetNearbyInteractTargets(Targets);
	for (const AInteract* Candidate : Targets) { if (Candidate == Target) { return true; } }
	return false;
}

//-----------------------------------------------------
//‰e‰ğœ‚âÚGI—¹Œã‚Ì‘ÎÛ‚ğXV
//-----------------------------------------------------
void APlayerCharacter::RefreshInteractTarget()
{
	m_hitInteractOb = FindInteractTarget();
	m_bHitIntteractObject = IsValid(m_hitInteractOb);
}
