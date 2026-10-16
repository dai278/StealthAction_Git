//----------------------------------------------------------
// ŠT—v				FƒvƒŒƒCƒ„[ƒLƒƒƒ‰‚ğ§Œä‚·‚éCharacterƒIƒuƒWƒFƒNƒg
// XV“ú@@@@ @F11/19@
//----------------------------------------------------------

//BP‚Åg‚¦‚é‚©‚Í“úîà‚Ì“Æ’f‚Æ•ÎŒ©‚ÅŒˆ‚ß‚é‚Ì‚ÅŒã‚Å•K—v‚É‰‚¶‚Ä•ÏX‚µ‚Ä—Ç‚µ
//ƒJƒƒ‰‚Æ•½sˆÚ“®‚ÌÅ’áŒÀ‚­‚ç‚¢‚µ‚©‘‚«‚Ü‚¹‚ñ


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameInstance/PlayDataInfo.h"//ƒvƒŒƒCƒf[ƒ^\‘¢‘Ì‚ª•¡”‚ ‚é‚à‚ÌPlayerInfo‚Ì‚İg—p
#include "Damage/IDamage.h"
#include "Camera/CameraInfo.h"
#include "State/Player/PlayerStatus.h"

#include "PlayerCharacter.generated.h"

//‘O•ûéŒ¾
class USpringArmComponent;
class UCameraComponent;
class UMaterialParamenterCooection;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;  
class UPrimitiveComponent;
class UNoiseListenerComponent;
class UEnemyManager;//ƒGƒlƒ~[‚ÌŠî’êƒNƒ‰ƒX‚É‚·‚é“úîà•ÏX
class AEnemyBase;
class UExtendedSpotLightManager;//Šg’£ƒXƒ|ƒbƒgƒ‰ƒCƒgƒ}ƒl[ƒWƒƒ[‚Ì‘O•ûéŒ¾
class USwordAttackComponent;//ƒ\[ƒhUŒ‚ƒRƒ“ƒ|[ƒlƒ“ƒg‚Ì‘O•ûéŒ¾
class UCameraFocusDirectorComponent;//ƒJƒƒ‰ƒtƒH[ƒJƒXƒfƒBƒŒƒNƒ^[ƒRƒ“ƒ|[ƒlƒ“ƒg‚Ì‘O•ûéŒ¾
class AInteract;
class UPlayerStateMachine;
class UPlayerLocomotionState;
class UPlayerAttackState;
class UPlayerDamageState;
class UPlayerShadowState;
class UPlayerDeadState;
class UPlayerInteractState;
class UPlayerInteractAnimationState;

//ƒvƒŒƒCƒ„[‚ÌƒJƒƒ‰‹“_ó‘Ô
//enum class‚ÍƒNƒ‰ƒXŠO‚É‘‚©‚È‚¢‚Æ‚¢‚¯‚È‚¢‚ç‚µ‚¢‚µA‚±‚Ì‘‚«•û‚ª—Ç‚¢‚ç‚µ‚¢
//ˆÃEƒV[ƒ“‚Æ‚©ƒJƒƒ‰•Ï‚¦‚é‚©‚à‚µ‚ê‚È‚¢‚©‚ç¡‚Í2í‚µ‚©‚È‚¢‚¯‚Çenum‚Å
UENUM(BlueprintType)
enum class ECameraStatus : uint8
{
	ThirdPerson      UMETA(DisplayName = "Third Person"),//OlÌ
	TopDownView      UMETA(DisplayName = "Top Down View"),//˜ëáÕ
	InShadow		UMETA(DisplayName = "InShadow"),//‰e
	Crouch			UMETA(DisplayName = "Crouch"),//‚µ‚á‚ª‚İ
	Dash			UMETA(DisplayName = "Dash"),//ƒ_ƒbƒVƒ…
	SneakKill		UMETA(DisplayName = "SneakKill"),//ˆÃE
	Num			 UMETA(DisplayName = "Num")//”Œv‘ª—pAUMETA(DisplayName = "Num")‚ª•K—v‚©‚í‚©‚ç‚È‚¢‚¯‚Ç‘‚©‚È‚¢ƒoƒO‚è‚»‚¤‚È‹C‚ª‚µ‚½‚©‚ç‘‚¢‚Ä‚Ü‚·
};

//ƒvƒŒƒCƒ„[‚Ìó‘Ô
//v‚¢‚Â‚¢‚½‚Ì‚©‚¢‚½
//‚¢‚ç‚È‚¢‚È‚çÁ‚µ‚Ä•K—v‚È‚ç‘‚â‚µ‚Ä
//enum‚ÅŠÇ—‚µ‚È‚­‚Ä‚à‚¢‚¢‚¯‚Ç‰eó‘Ô‚É‚à‚Å‚«‚é‚æ‚¤‚ÈİŒv‚É‚µ‚Æ‚¢‚Ä




//ˆÃEŠJnƒCƒxƒ“ƒgƒfƒBƒXƒpƒbƒ`ƒƒ[
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSneakKillStarted); // ˆø”‚È‚µ—á


UCLASS()
class STEALTHACTION_API APlayerCharacter : public ACharacter,public IDamageable
{
	////ƒJƒƒ‰‹“_İ’è\‘¢‘Ì
	//struct FCameraViewSetting
	//{
	//	FRotator rotator;//‰ñ“]
	//	float fieldOfView;//‹–ìŠp
	//	float springArmLength;//’·‚³
	//};

	GENERATED_BODY()

	friend class UPlayerStateMachine;
	friend class UPlayerLocomotionState;
	friend class UPlayerAttackState;
	friend class UPlayerDamageState;
	friend class UPlayerShadowState;
	friend class UPlayerDeadState;
	friend class UPlayerInteractState;
	friend class UPlayerInteractAnimationState;

public:

	//UŒ‚ŠJnƒR[ƒ‹ƒoƒbƒN
// BP‘¤‚ÅƒCƒxƒ“ƒg‚ğ’Ç‰Á‚Å‚«‚éiAdd Event / Bindj
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSneakKillStarted OnSneakKillStarted;


	//ƒRƒ“ƒXƒgƒ‰ƒNƒ^
	APlayerCharacter();

protected:
	//ƒQ[ƒ€ƒXƒ^[ƒg–”‚Í¶¬ŒÄ‚Î‚ê‚é‰Šú‰»ˆ—
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// XV
	virtual void Tick(float _deltaTime) override;

	// Še“ü—ÍŠÖŒWƒƒ\ƒbƒh‚Æ‚ÌƒoƒCƒ“ƒhˆ—
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Õ“Ë‚µ‚½uŠÔ‚ÌƒCƒxƒ“ƒgŠÖ”
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	//Õ“Ë‚ªI‚í‚Á‚½uŠÔ‚ÌƒCƒxƒ“ƒgŠÖ”
	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION()
	FPlayerInfo GetPlayerInfo();

	// PlayerCharacter.h
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSneakKillStarted_BP();   // BP‚Å’†g‚ğ‘‚­

	// ƒLƒ‹ƒAƒjƒ[ƒVƒ‡ƒ“‚ÌƒJƒƒ‰§Œä•ÏXƒCƒxƒ“ƒg
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetEventCameraChange(FCameraViewSetting _viewSetting);   
	
	//ƒ_ƒ[ƒWˆ—
	void OnDamage(int32 Damage, FVector KnockBackVec, bool bSneakKill)override;

private:
	//ƒJƒƒ‰XV
	void UpdateCamera(float _deltaTime);

	//ƒJƒƒ‰èU‚ê•â³XVˆ—
	void UpdateCameraShake(float _deltaTime);

	//ƒJƒƒ‰ƒRƒŠƒWƒ‡ƒ“”»’èˆ—
	void UpdateCameraCollision();

	//ƒJƒƒ‰‹“_Ø‚è‘Ö‚¦
	void ViewpointSwitching(float _deltaTime);

	//“G‚É‚Î‚ê‚Ä‚¢‚é‚©‚È‚Ç‚ÌQÆ—p
	void UpdateCheckEnemyDetection();

private:
	//•à‚«ó‘Ô‚ÌXVˆ—
	void UpdateMove(const bool _bInShadow=false);

	//–³“GŠÔ‚ÌXVˆ—
	void UpdateInvincibleTime(float _deltaTime);

	//ƒCƒ“ƒ^ƒ‰ƒNƒg‚ÌXVˆ—
	void UpdateInteract(float _deltaTime);

	//‰eó‘Ô‚ÌXVˆ—
	void UpdateShadow(float _deltaTime);

	//ƒXƒ^ƒ~ƒiÁ”ï
	void StaminaConsumption(float _deltaTime);

	//ƒXƒ^ƒ~ƒi‰ñ•œ
	void StaminaRecovery(const float& _deltaTime);

	//ƒWƒƒƒ“ƒvó‘Ô‚ÌXVˆ—
	void UpdateJump(float _deltaTime);

	//‰e‚©‚ç’Êíó‘Ô‚Ö•Ï‰»
	void TransformationShadowToIdle(const bool _bLightHit = false);
	//‰eó‘Ô‚Ö•Ï‰»
	void TransformationToShadow();
	//‰eó‘Ô‰ğœˆ—
	void CancellationShadow(const EPlayerStatus& _status);

	//‘«Œ³À•W‚Ìæ“¾
	FVector GetFeetLocation()const;


public:

	//UŒ‚I—¹ƒR[ƒ‹ƒoƒbƒN
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void OnAttackEnd();


	
	//ƒJƒƒ‰ƒtƒH[ƒJƒXŠJnˆ—
	UFUNCTION( Category = "Camera")
	void StartCameraFocus(AActor* const _cameraActor, float _blendTime);

	//ƒJƒƒ‰ƒtƒH[ƒJƒXI—¹ˆ—
	void EndCameraFocus(const float& _blendTime);

	//¶‚«‚Ä‚¢‚é‚©
	UFUNCTION(BlueprintPure, Category = "Info")
	bool IsAlive() const { return m_playerInfo.isAlive; }

	UFUNCTION(BlueprintPure, Category = "state")
	EPlayerStatus GetPlayerStatus() const;

	UFUNCTION(BlueprintPure, Category = "state")
	UPlayerStateMachine* GetPlayerStateMachine() const;

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsCrouch() const { return m_bIsCrouch; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool CanAttack() const { return m_bCanAttack; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsShadow() const { return GetPlayerStatus() == EPlayerStatus::InShadow; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsDash() const { return m_bDash; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsInvincible() const { return m_bInvincible; }

	UFUNCTION(BlueprintCallable,Category = "Animation")
	void OnInteractAnimationEnd() {};

	//HPæ“¾
	int32 GetPlayerHP() const { return m_playerInfo.hp; }

protected:
	//---“ü—Í—p---
	// Enhanced Input ŠÖ”
	void Enhanced_Move(const FInputActionValue& Value);		//ˆÚ“®	
	void Enhanced_EndMove(const FInputActionValue& Value);
	void Enhanced_OnInputDash(const FInputActionValue& Value);//ƒ_ƒbƒVƒ…“ü—ÍŠJn
	void Enhanced_EndInputDash(const FInputActionValue& Value);//ƒ_ƒbƒVƒ…“ü—ÍI—¹


	void Enhanced_MoveCrouch(const FInputActionValue& Value);		//‚µ‚á‚ª‚İ
	void Enhanced_MoveJump(const FInputActionValue& Value);			//ƒWƒƒƒ“ƒv
	void Enhanced_Attack(const FInputActionValue& Value);			//UŒ‚
	void Enhanced_InShadow(const FInputActionValue& Value);			//‰eö‚è
	void Enhanced_changeWeapon(const FInputActionValue& Value);		//•Ší•ÏX
	void Enhanced_Interact(const FInputActionValue& Value);			//ƒCƒ“ƒ^ƒ‰ƒNƒg

	void Enhanced_CameraPitch(const FInputActionValue& Value);		//ƒJƒƒ‰ƒsƒbƒ`
	void Enhanced_CameraYaw(const FInputActionValue& Value);		//ƒJƒƒ‰ƒˆ[
	void Enhanced_CameraReset(const FInputActionValue& Value);		//ƒJƒƒ‰ƒŠƒZƒbƒg
	void Enhanced_CameraSwitch(const FInputActionValue& Value);		//‹“_•ÏX

	void Enhanced_CameraPitchReleased(const FInputActionValue& Value);	//ƒJƒƒ‰ƒsƒbƒ`
	void Enhanced_CameraYawReleased(const FInputActionValue& Value);	//ƒJƒƒ‰ƒˆ[


protected:
	// ƒfƒtƒHƒ‹ƒg‚Åg‚¤ Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	//ˆÚ“®
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveIA;

	//ƒWƒƒƒ“ƒv
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveJumpIA;

	//ƒ_ƒbƒVƒ…
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveDashIA;

	//‚µ‚á‚ª‚İ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveCrouchIA;

	//ƒJƒƒ‰Pitch
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_cameraPitchIA;

	//ƒJƒƒ‰YAW
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_cameraYawIA;

	//ƒJƒƒ‰ƒŠƒZƒbƒg
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_cameraResetIA;

	//ƒJƒƒ‰‹“_•ÏXƒtƒ‰ƒO
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_cameraSwitchIA;

	//•Ší•ÏX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_changeWeaponIA;

	//UŒ‚
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_attackIA;

	//‰eö‚è
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_inShadowIA;

	//ƒCƒ“ƒ^ƒ‰ƒNƒg
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_interactIA;

public:
	//‰eó‘Ô‚©H
	bool IsInShadow()const;

	//’n–Ê‚Â‚¢‚½‚ÉŒÄ‚Î‚ê‚éƒR[ƒ‹ƒoƒbƒNŠÖ”
	virtual void Landed(const FHitResult& Hit) override;

	//‰eö‚èŠÔæ“¾
	float GetShadowTimer() const { return m_staminaTimer; }

	//Å‘å‰eö‚èŠÔæ“¾
	float GetMaxShadowTime() const { return m_maxStamina; }

	private:
	//ó‘Ô•ÏXˆ—
	bool ChangePlayerStatus(const EPlayerStatus& _newStatus);
	void RefreshLocomotionState();
	void FinishAttackState();
	void EnterShadowState();
	void ExitShadowState();

private:
	bool IsUpPosWall(const FVector& _startPos)const;

	//ƒ_ƒbƒVƒ…I—¹ˆ—
	void EndDash();


private:

	UPROPERTY(EditDefaultsOnly, Category = "State Machine", meta = (DisplayName = "Player State Machine Class"))
	TSubclassOf<UPlayerStateMachine> m_stateMachineClass;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerStateMachine> m_stateMachine;

	//BlueprintRedOnly‚Éw’è‚µ‚Ä‚¢‚é‚½‚ßƒuƒ‹[ƒvƒŠƒ“ƒg‚ÅŒ©‚ê‚é‚ªA•ÒW‚Í‚Å‚«‚È‚¢i•Ï”•ÛŒìj
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* m_pSpringArm;				//ƒXƒvƒŠƒ“ƒOƒA[ƒ€

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_pCamera;					//ƒJƒƒ‰

	UPROPERTY()
	TArray<AActor*>m_pHitActors;					//ƒqƒbƒgAcotr(SpringArm’¼ü‚ÆÕ“Ë‚µ‚Ä‚¢‚éActor

	UPROPERTY(EditAnywhere, Category = "Camera")
	ECameraStatus m_cameraStatus;					//ƒJƒƒ‰‚ÌƒXƒe[ƒg

	UPROPERTY(EditAnywhere, Category = "Camera")
	FVector2D m_cameraPitchLimit;					//ƒJƒƒ‰‚Ìƒsƒbƒ`”ÍˆÍ

	UPROPERTY(EditAnywhere, Category = "Camera")
	float m_cameraRotateSpeed;						//ƒJƒƒ‰‰ñ“]‘¬“x

	UPROPERTY()
	UMaterialParameterCollection* m_pMPC;			//ƒ}ƒeƒŠƒAƒ‹ƒpƒ‰ƒ[ƒ^ƒRƒŒƒNƒVƒ‡ƒ“

	UPROPERTY(EditDefaultsOnly, Category = "Status", meta = (DisplayName = "Initial Player Status"))
	EPlayerStatus m_status; // Initial status only; runtime state lives in m_stateMachine.

	UPROPERTY(EditAnywhere, Category = "Move")
	float m_WalkSpeed;								//•à‚«ˆÚ“®—Ê

	UPROPERTY(EditAnywhere, Category = "Move")
	float m_DashSpeed;								//ƒ_ƒbƒVƒ…ˆÚ“®—Ê

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float m_dashStaminaConsumptionMagnification;

	UPROPERTY(EditAnywhere, Category = "Move")
	float m_CrouchSpeed;								//‚µ‚á‚ª‚İˆÚ“®—Ê

	UPROPERTY(EditAnywhere, Category = "Move")
	float m_JumpVector;								//ƒWƒƒƒ“ƒv—Ê

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMesh* m_defaultMesh;					//’Êí‚ÌƒƒbƒVƒ…

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMesh* m_isShadowMesh;					//‰e‚É“ü‚Á‚Ä‚é‚ÌƒƒbƒVƒ…

	UPROPERTY(EditAnywhere, Category = "Collision")
	UCapsuleComponent* m_Capsule;						//ƒJƒvƒZƒ‹ƒRƒ“ƒ|[ƒlƒ“ƒg

	UPROPERTY(EditAnywhere, Category = "Collision")
	float m_capsuleRadius;							//ƒJƒvƒZƒ‹”¼Œa

	UPROPERTY(EditAnywhere, Category = "Collision")
	float m_capsuleHeight;							//ƒJƒvƒZƒ‹‚‚³

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float m_maxStamina;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float m_staminaTimer;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float m_staminaRecoveryMagnification;

	bool m_isStaminaDepleted;//ƒXƒ^ƒ~ƒiØ‚êó‘Ô‚©H

	UPROPERTY(EditAnywhere, Category = "Attack")
	float m_attackRange;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float m_attackRadius;

	UPROPERTY(EditAnywhere, Category = "Attack")
	int m_sneakKillDamage;

	UPROPERTY(EditAnywhere, Category = "Attack")
	int m_NormalAttack;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float m_attackCooldown;

	bool m_bDash;


	//ƒJƒƒ‰ƒtƒH[ƒJƒX—p•Ï”
	AActor* m_pSaveCameraActor;//ƒtƒH[ƒJƒX‘O‚ÌƒJƒƒ‰ƒAƒNƒ^[•Û‘¶—p

	UPROPERTY(EditAnywhere, Category = "CameraFocus")
	UCameraFocusDirectorComponent* m_pCameraFocusDirector;//ƒJƒƒ‰ƒtƒH[ƒJƒXƒfƒBƒŒƒNƒ^[ƒRƒ“ƒ|[ƒlƒ“ƒg

	//UPROPERTY(EditAnywhere, Category = "weapon");
	//USwordAttackComponent* m_pSword;			//ƒ\[ƒhUŒ‚ƒRƒ“ƒ|[ƒlƒ“ƒg
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USwordAttackComponent* m_sword;	//ƒ\[ƒhUŒ‚ƒRƒ“ƒ|[ƒlƒ“ƒg

	bool m_bInvincible;//–³“Gó‘Ô‚©
	float m_invincibleTimer;//–³“GŠÔ

	FVector m_knockBackVelocity;//ƒmƒbƒNƒoƒbƒN‘¬“x
	float m_damageTime;//ƒ_ƒ[ƒWŠÔ

	UPROPERTY(EditAnywhere, Category = "Damage")
	float m_invincibleTimeLimit;//–³“GŠÔ§ŒÀ


	FTimerHandle AttackCooldownHandle;

	bool m_bCameraSwitching;						//ƒJƒƒ‰‹“_Ø‚è‘Ö‚¦’†‚©H
	bool m_bCanControl;								//‘€ì‰Â”\ó‘Ô‚©

	UPROPERTY(VisibleAnywhere, Category = "Sahdow")
	bool m_bOnShadow;                               //‰e‚Ìã‚É‚¢‚é‚©

	bool m_bUsingMesh;								//g‚Á‚Ä‚éƒƒbƒVƒ…‚ğŠÇ—‚·‚éƒtƒ‰ƒO
	bool m_bIsCrouch;
	bool m_bCanAttack;
	bool m_bSneakKill;									//ƒXƒj[ƒNƒLƒ‹‚µ‚Ä‚¢‚é‚©
	float m_attackCount;//UŒ‚ŠÔ

	bool m_bJumping;									//ƒWƒƒƒ“ƒv’†‚©
	float m_jumpTimer;

	FVector2D m_charaMoveInput;						//ƒLƒƒƒ‰ˆÚ“®“ü—Í—Ê
	FVector2D m_cameraRotateInput;					//ƒJƒƒ‰‰ñ“]—Ê

	UPROPERTY(EditAnywhere, Category = "Camera")
	FCameraViewSetting m_cameraInitPos[(int)ECameraStatus::Num];	//Še‹“_•ÏXÅ‰‚Ì‹“_

	UPROPERTY(EditAnywhere, Category = "Camera")
	float m_maxOffsetY;	//ƒIƒtƒZƒbƒgYÀ•W‚ÌÅ‘å’l

	float m_OutcameraInputTimer;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float m_initOffsetY;

	TArray <AActor*> m_hitActors;//Õ“Ë‘Îˆ‚ÌƒAƒhƒŒƒX

	//ƒCƒ“ƒ^ƒ‰ƒNƒg‰Â‚ÈƒIƒuƒWƒFƒNƒg‘€ì”ÍˆÍ“à‚©
	bool m_bHitIntteractObject;
	//ƒCƒ“ƒ^ƒ‰ƒNƒg‚·‚éƒIƒuƒWƒFƒNƒg
	AInteract* m_hitInteractOb;
	//ƒCƒ“ƒ^ƒ‰ƒNƒg‚Ì‚·‚éêŠ
	FVector m_interactPos;
	//ƒCƒ“ƒ^ƒ‰ƒNƒgƒ^ƒCƒ}[
	bool m_bInteractionExecuted = false;
	float m_interactTimer;


	//“úîà•ÏX“_
	UEnemyManager* m_pEnemyManager;//ƒGƒlƒ~[ƒ}ƒl[ƒWƒƒ[–ˆƒtƒŒ[ƒ€ŒŸõ‚Íd‚¢‚Ì‚Å
	//Šg’£ƒXƒ|ƒbƒgƒ‰ƒCƒgƒ}ƒl[ƒWƒƒ[‚ÌƒAƒhƒŒƒX
	UExtendedSpotLightManager* m_pExtendedSpotLightManager;
	
	UPROPERTY(EditAnywhere, Category = "Enemy")
	AEnemyBase* m_pNearestEnemy;//ˆê”Ô‹ß‚¢“G‚Ìƒ|ƒCƒ“ƒ^
	
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	AEnemyBase* GetNearestEnemy() { return m_pNearestEnemy; }


	//ƒfƒoƒbƒN—p
	UNoiseListenerComponent* noise;
	public:
	void OnNoiseHeard(const int& _noiseVolume, const FVector& _pos);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FPlayerInfo m_playerInfo; //ƒvƒŒƒCƒ„[î•ñ\‘¢‘Ì

	bool bIsCameraForced;//ƒJƒƒ‰‹­§ˆÚ“®’†‚©

	//ƒL[ƒAƒCƒeƒ€‚ğæ“¾‚µ‚Ä‚¢‚é‚©
	bool bHasKeyItem;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetHasKeyItem(bool bHasItem) { bHasKeyItem = bHasItem; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	bool GetHasKeyItem() const { return bHasKeyItem; }
	
	//ŠJn‰‰o—pƒJƒƒ‰
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class ACameraActor> m_focusMovieCamera ;


	// ‰eƒGƒtƒFƒNƒg‚ÌƒNƒ‰ƒXiBP‚Å‚àw’è‚µ‚½‚¢‚È‚çj
	UPROPERTY(EditAnywhere, Category = "Shadow")
	TSubclassOf<AActor> m_ShadowEffectClass;

	// À‘ÌiƒvƒŒƒCƒ„[‚É‚Ô‚ç‰º‚ª‚éj
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shadow", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UChildActorComponent> m_ShadowEffectChild;


	
public:
		//Œ®æ“¾‚ÌŠÖ”
	void OnGetKeyItem();

	//‰eó‘ÔAÅŒã‚Ì•Ç‚ª–³‚¢êŠ‚Ì‹L˜^—p
	FVector m_saveLastNotUpWallPos;


	protected:
		//ƒJƒƒ‰•Ç‰ñ”ğ—p•Ï”
		UPROPERTY(EditAnywhere, Category = "Camera|WallAvoid")
		float WallAvoidSideOffset = 35.f;   // ‰¡‚É“¦‚ª‚·—Êicmj

		UPROPERTY(EditAnywhere, Category = "Camera|WallAvoid")
		float WallAvoidInterpSpeed = 12.f;  // ’Ç]‘¬“x

		UPROPERTY(EditAnywhere, Category = "Camera|WallAvoid")
		float WallAvoidTraceRadius = 8.f;   // ƒXƒtƒBƒA”¼Œaicmj

		UPROPERTY(EditAnywhere, Category = "Camera|WallAvoid")
		TEnumAsByte<ECollisionChannel> WallAvoidChannel = ECC_Camera;

		float CurrentSideOffsetY = 0.f;

		//ƒJƒƒ‰•Ç‰ñ”ğˆ—
		void UpdateCamerawallSiseOffset(float _deltaTime);


		UPROPERTY(EditAnywhere, Category = "Cranch")
		float CranchCollisionRatio;

		UPROPERTY(EditAnywhere, Category = "Shadow")
		float ShadowCollisionRatio;

};
