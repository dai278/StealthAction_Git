//----------------------------------------------------------
// 概要				：プレイヤーキャラを制御するCharacterオブジェクト
// 更新日　　　　 　：11/19　
//----------------------------------------------------------

//BPで使えるかは日髙の独断と偏見で決めるので後で必要に応じて変更して良し
//カメラと平行移動の最低限くらいしか書きません


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameInstance/PlayDataInfo.h"//プレイデータ構造体が複数あるものPlayerInfoのみ使用
#include "Damage/IDamage.h"
#include "Camera/CameraInfo.h"

#include "PlayerCharacter.generated.h"

//前方宣言
class USpringArmComponent;
class UCameraComponent;
class UMaterialParamenterCooection;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;  
class UPrimitiveComponent;
class UNoiseListenerComponent;
class UEnemyManager;//エネミーの基底クラスにする日髙変更
class AEnemyBase;
class UExtendedSpotLightManager;//拡張スポットライトマネージャーの前方宣言
class USwordAttackComponent;//ソード攻撃コンポーネントの前方宣言
class UCameraFocusDirectorComponent;//カメラフォーカスディレクターコンポーネントの前方宣言
class AInteract;

//プレイヤーのカメラ視点状態
//enum classはクラス外に書かないといけないらしいし、この書き方が良いらしい
//暗殺シーンとかカメラ変えるかもしれないから今は2種しかないけどenumで
UENUM(BlueprintType)
enum class ECameraStatus : uint8
{
	ThirdPerson      UMETA(DisplayName = "Third Person"),//三人称
	TopDownView      UMETA(DisplayName = "Top Down View"),//俯瞰
	InShadow		UMETA(DisplayName = "InShadow"),//影
	Crouch			UMETA(DisplayName = "Crouch"),//しゃがみ
	Dash			UMETA(DisplayName = "Dash"),//ダッシュ
	SneakKill		UMETA(DisplayName = "SneakKill"),//暗殺
	Num			 UMETA(DisplayName = "Num")//数計測用、UMETA(DisplayName = "Num")が必要かわからないけど書かないバグりそうな気がしたから書いてます
};

//プレイヤーの状態
//思いついたのかいた
//いらないなら消して必要なら増やして
//enumで管理しなくてもいいけど影状態にもできるような設計にしといて
UENUM(BlueprintType)
enum class EPlayerStatus :uint8
{
	Idle         UMETA(DisplayName = "Idle"),
	Walk         UMETA(DisplayName = "Walk"),
	Dash         UMETA(DisplayName = "Dash"),
	Jump         UMETA(DisplayName = "Jump"),
	Crouch       UMETA(DisplayName = "Crouch"),
	Attack       UMETA(DisplayName = "Attack"),
	Damage       UMETA(DisplayName = "Damage"),
	Dead         UMETA(DisplayName = "Dead"),
	InShadow     UMETA(DisplayName = "InShadow"),
	Interact     UMETA(DisplayName = "Interact"),
	InteractAnimation UMETA(DisplayName = "InteractAnimation"),
};



//暗殺開始イベントディスパッチャー
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSneakKillStarted); // 引数なし例


UCLASS()
class STEALTHACTION_API APlayerCharacter : public ACharacter,public IDamageable
{
	////カメラ視点設定構造体
	//struct FCameraViewSetting
	//{
	//	FRotator rotator;//回転
	//	float fieldOfView;//視野角
	//	float springArmLength;//長さ
	//};

	GENERATED_BODY()

public:

	//攻撃開始コールバック
// BP側でイベントを追加できる（Add Event / Bind）
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSneakKillStarted OnSneakKillStarted;


	//コンストラクタ
	APlayerCharacter();

protected:
	//ゲームスタート又は生成時呼ばれる初期化処理
	virtual void BeginPlay() override;

public:
	// 更新
	virtual void Tick(float _deltaTime) override;

	// 各入力関係メソッドとのバインド処理
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//衝突した瞬間のイベント関数
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	//衝突が終わった瞬間のイベント関数
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
	void OnSneakKillStarted_BP();   // BPで中身を書く

	// キルアニメーションのカメラ制御変更イベント
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetEventCameraChange(FCameraViewSetting _viewSetting);   
	
	//ダメージ処理
	void OnDamage(int32 Damage, FVector KnockBackVec, bool bSneakKill)override;

private:
	//カメラ更新
	void UpdateCamera(float _deltaTime);

	//カメラ手振れ補正更新処理
	void UpdateCameraShake(float _deltaTime);

	//カメラコリジョン判定処理
	void UpdateCameraCollision();

	//カメラ視点切り替え
	void ViewpointSwitching(float _deltaTime);

	//敵にばれているかなどの参照用
	void UpdateCheckEnemyDetection();

private:
	//アイドル状態の更新処理
	void UpdateIdle();

	//歩き状態の更新処理
	void UpdateMove(const bool _bInShadow=false);

	//しゃがみ状態の更新処理
	void UpdateCrouch();

	//攻撃状態の更新処理
	void UpdateAttack();

	//ダメージ状態の更新処理
	void UpdateDamaged();

	//やられ状態の更新処理
	void UpdateDead(float _deltaTime);

	//無敵時間の更新処理
	void UpdateInvincibleTime(float _deltaTime);

	//インタラクトの更新処理
	void UpdateInteract(float _deltaTime);

	//影状態の更新処理
	void UpdateShadow(float _deltaTime);

	//スタミナ消費
	void StaminaConsumption(float _deltaTime);

	//スタミナ回復
	void StaminaRecovery(const float& _deltaTime);

	//ジャンプ状態の更新処理
	void UpdateJump(float _deltaTime);

	//影から通常状態へ変化
	void TransformationShadowToIdle(const bool _bLightHit = false);
	//影状態へ変化
	void TransformationToShadow();
	//影状態解除処理
	void CancellationShadow(const EPlayerStatus& _status);

	//足元座標の取得
	FVector GetFeetLocation()const;


public:

	//攻撃終了コールバック
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void OnAttackEnd();


	
	//カメラフォーカス開始処理
	UFUNCTION( Category = "Camera")
	void StartCameraFocus(AActor* const _cameraActor, float _blendTime);

	//カメラフォーカス終了処理
	void EndCameraFocus(const float& _blendTime);

	//生きているか
	UFUNCTION(BlueprintPure, Category = "Info")
	bool IsAlive() const { return m_playerInfo.isAlive; }

	UFUNCTION(BlueprintPure, Category = "state")
	EPlayerStatus GetPlayerStatus() const { return m_status; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsCrouch() const { return m_bIsCrouch; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool CanAttack() const { return m_bCanAttack; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsShadow() const { return m_status == EPlayerStatus::InShadow; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsDash() const { return m_bDash; }

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsInvincible() const { return m_bInvincible; }

	UFUNCTION(BlueprintCallable,Category = "Animation")
	void OnInteractAnimationEnd() {};

	//HP取得
	int32 GetPlayerHP() const { return m_playerInfo.hp; }

protected:
	//---入力用---
	// Enhanced Input 関数
	void Enhanced_Move(const FInputActionValue& Value);		//移動	
	void Enhanced_OnInputDash(const FInputActionValue& Value);//ダッシュ入力開始
	void Enhanced_EndInputDash(const FInputActionValue& Value);//ダッシュ入力終了


	void Enhanced_MoveCrouch(const FInputActionValue& Value);		//しゃがみ
	void Enhanced_MoveJump(const FInputActionValue& Value);			//ジャンプ
	void Enhanced_Attack(const FInputActionValue& Value);			//攻撃
	void Enhanced_InShadow(const FInputActionValue& Value);			//影潜り
	void Enhanced_changeWeapon(const FInputActionValue& Value);		//武器変更
	void Enhanced_Interact(const FInputActionValue& Value);			//インタラクト

	void Enhanced_CameraPitch(const FInputActionValue& Value);		//カメラピッチ
	void Enhanced_CameraYaw(const FInputActionValue& Value);		//カメラヨー
	void Enhanced_CameraReset(const FInputActionValue& Value);		//カメラリセット
	void Enhanced_CameraSwitch(const FInputActionValue& Value);		//視点変更

	void Enhanced_CameraPitchReleased(const FInputActionValue& Value);	//カメラピッチ
	void Enhanced_CameraYawReleased(const FInputActionValue& Value);	//カメラヨー


protected:
	// デフォルトで使う Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	//移動
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveIA;

	//ジャンプ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveJumpIA;

	//ダッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveDashIA;

	//しゃがみ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveCrouchIA;

	//カメラPitch
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_cameraPitchIA;

	//カメラYAW
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_cameraYawIA;

	//カメラリセット
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_cameraResetIA;

	//カメラ視点変更フラグ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_cameraSwitchIA;

	//武器変更
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_changeWeaponIA;

	//攻撃
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_attackIA;

	//影潜り
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_inShadowIA;

	//インタラクト
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_interactIA;

public:
	//影状態か？
	bool IsInShadow()const;

	//地面ついた時に呼ばれるコールバック関数
	virtual void Landed(const FHitResult& Hit) override;

	//影潜り時間取得
	float GetShadowTimer() const { return m_staminaTimer; }

	//最大影潜り時間取得
	float GetMaxShadowTime() const { return m_maxStamina; }

	private:
	//状態変更処理
	void ChangePlayerStatus(const EPlayerStatus& _newStatus);

private:
	bool IsUpPosWall(const FVector& _startPos)const;

	//ダッシュ終了処理
	void EndDash();


private:

	//BlueprintRedOnlyに指定しているためブループリントで見れるが、編集はできない（変数保護）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* m_pSpringArm;				//スプリングアーム

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_pCamera;					//カメラ

	UPROPERTY()
	TArray<AActor*>m_pHitActors;					//ヒットAcotr(SpringArm直線と衝突しているActor

	UPROPERTY(EditAnywhere, Category = "Camera")
	ECameraStatus m_cameraStatus;					//カメラのステート

	UPROPERTY(EditAnywhere, Category = "Camera")
	FVector2D m_cameraPitchLimit;					//カメラのピッチ範囲

	UPROPERTY(EditAnywhere, Category = "Camera")
	float m_cameraRotateSpeed;						//カメラ回転速度

	UPROPERTY()
	UMaterialParameterCollection* m_pMPC;			//マテリアルパラメータコレクション

	UPROPERTY(EditAnywhere, Category = "Status")
	EPlayerStatus m_status;							//プレイヤー状態

	UPROPERTY(EditAnywhere, Category = "Move")
	float m_WalkSpeed;								//歩き移動量

	UPROPERTY(EditAnywhere, Category = "Move")
	float m_DashSpeed;								//ダッシュ移動量

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float m_dashStaminaConsumptionMagnification;

	UPROPERTY(EditAnywhere, Category = "Move")
	float m_CrouchSpeed;								//しゃがみ移動量

	UPROPERTY(EditAnywhere, Category = "Move")
	float m_JumpVector;								//ジャンプ量

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMesh* m_defaultMesh;					//通常のメッシュ

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMesh* m_isShadowMesh;					//影に入ってる時のメッシュ

	UPROPERTY(EditAnywhere, Category = "Collision")
	UCapsuleComponent* m_Capsule;						//カプセルコンポーネント

	UPROPERTY(EditAnywhere, Category = "Collision")
	float m_capsuleRadius;							//カプセル半径

	UPROPERTY(EditAnywhere, Category = "Collision")
	float m_capsuleHeight;							//カプセル高さ

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float m_maxStamina;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float m_staminaTimer;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float m_staminaRecoveryMagnification;

	bool m_isStaminaDepleted;//スタミナ切れ状態か？

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


	//カメラフォーカス用変数
	AActor* m_pSaveCameraActor;//フォーカス前のカメラアクター保存用

	UPROPERTY(EditAnywhere, Category = "CameraFocus")
	UCameraFocusDirectorComponent* m_pCameraFocusDirector;//カメラフォーカスディレクターコンポーネント

	//UPROPERTY(EditAnywhere, Category = "weapon");
	//USwordAttackComponent* m_pSword;			//ソード攻撃コンポーネント
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USwordAttackComponent* m_sword;	//ソード攻撃コンポーネント

	bool m_bInvincible;//無敵状態か
	float m_invincibleTimer;//無敵時間

	FVector m_knockBackVelocity;//ノックバック速度
	float m_damageTime;//ダメージ時間

	UPROPERTY(EditAnywhere, Category = "Damage")
	float m_invincibleTimeLimit;//無敵時間制限


	FTimerHandle AttackCooldownHandle;

	bool m_bCameraSwitching;						//カメラ視点切り替え中か？
	bool m_bCanControl;								//操作可能状態か

	UPROPERTY(VisibleAnywhere, Category = "Sahdow")
	bool m_bOnShadow;                               //影の上にいるか

	bool m_bUsingMesh;								//使ってるメッシュを管理するフラグ
	bool m_bIsCrouch;
	bool m_bCanAttack;
	bool m_bSneakKill;									//スニークキルしているか
	float m_attackCount;//攻撃時間

	bool m_bJumping;									//ジャンプ中か
	float m_jumpTimer;

	FVector2D m_charaMoveInput;						//キャラ移動入力量
	FVector2D m_cameraRotateInput;					//カメラ回転量

	UPROPERTY(EditAnywhere, Category = "Camera")
	FCameraViewSetting m_cameraInitPos[(int)ECameraStatus::Num];	//各視点変更時最初の視点

	UPROPERTY(EditAnywhere, Category = "Camera")
	float m_maxOffsetY;	//オフセットY座標の最大値

	float m_OutcameraInputTimer;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float m_initOffsetY;

	TArray <AActor*> m_hitActors;//衝突対処のアドレス

	//インタラクト可なオブジェクト操作範囲内か
	bool m_bHitIntteractObject;
	//インタラクトするオブジェクト
	AInteract* m_hitInteractOb;
	//インタラクトのする場所
	FVector m_interactPos;
	//インタラクトタイマー
	float m_interactTimer;


	//日髙変更点
	UEnemyManager* m_pEnemyManager;//エネミーマネージャー毎フレーム検索は重いので
	//拡張スポットライトマネージャーのアドレス
	UExtendedSpotLightManager* m_pExtendedSpotLightManager;
	
	UPROPERTY(EditAnywhere, Category = "Enemy")
	AEnemyBase* m_pNearestEnemy;//一番近い敵のポインタ
	
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	AEnemyBase* GetNearestEnemy() { return m_pNearestEnemy; }


	//デバック用
	UNoiseListenerComponent* noise;
	public:
	void OnNoiseHeard(const int& _noiseVolume, const FVector& _pos);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FPlayerInfo m_playerInfo; //プレイヤー情報構造体

	bool bIsCameraForced;//カメラ強制移動中か

	//キーアイテムを取得しているか
	bool bHasKeyItem;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetHasKeyItem(bool bHasItem) { bHasKeyItem = bHasItem; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	bool GetHasKeyItem() const { return bHasKeyItem; }
	
	//開始時演出用カメラ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class ACameraActor> m_focusMovieCamera ;


	// 影エフェクトのクラス（BPでも指定したいなら）
	UPROPERTY(EditAnywhere, Category = "Shadow")
	TSubclassOf<AActor> m_ShadowEffectClass;

	// 実体（プレイヤーにぶら下がる）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shadow", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UChildActorComponent> m_ShadowEffectChild;


	
public:
		//鍵取得時の関数
	void OnGetKeyItem();

	//影状態、最後の壁が無い場所の記録用
	FVector m_saveLastNotUpWallPos;


	protected:
		//カメラ壁回避用変数
		UPROPERTY(EditAnywhere, Category = "Camera|WallAvoid")
		float WallAvoidSideOffset = 35.f;   // 横に逃がす量（cm）

		UPROPERTY(EditAnywhere, Category = "Camera|WallAvoid")
		float WallAvoidInterpSpeed = 12.f;  // 追従速度

		UPROPERTY(EditAnywhere, Category = "Camera|WallAvoid")
		float WallAvoidTraceRadius = 8.f;   // スフィア半径（cm）

		UPROPERTY(EditAnywhere, Category = "Camera|WallAvoid")
		TEnumAsByte<ECollisionChannel> WallAvoidChannel = ECC_Camera;

		float CurrentSideOffsetY = 0.f;

		//カメラ壁回避処理
		void UpdateCamerawallSiseOffset(float _deltaTime);


		UPROPERTY(EditAnywhere, Category = "Cranch")
		float CranchCollisionRatio;

		UPROPERTY(EditAnywhere, Category = "Shadow")
		float ShadowCollisionRatio;

};
