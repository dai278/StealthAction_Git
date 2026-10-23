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
#include "State/Player/PlayerStatus.h"

#include "PlayerCharacter.generated.h"

class UPlayerStateBase;

//前方宣言
class USpringArmComponent;
class UCameraComponent;
class UMaterialParamenterCooection;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;  
class UPrimitiveComponent;
class UNoiseListenerComponent;
class AEnemyBase;
class USwordAttackComponent;//ソード攻撃コンポーネントの前方宣言
class UCameraFocusDirectorComponent;//カメラフォーカスディレクターコンポーネントの前方宣言
class AInteract;
class UPlayerStateMachine;
class UPlayerStateBase;
class UPlayerDashState;
class UPlayerCrouchState;
class UPlayerJumpState;
class UPlayerLocomotionState;
class UPlayerAttackState;
class UPlayerDamageState;
class UPlayerShadowState;
class UPlayerDeadState;
class UPlayerInteractState;
class UPlayerInteractAnimationState;

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

	friend class UPlayerStateMachine;
	friend class UPlayerStateBase;
	friend class UPlayerDashState;
	friend class UPlayerCrouchState;
	friend class UPlayerJumpState;
	friend class UPlayerLocomotionState;
	friend class UPlayerAttackState;
	friend class UPlayerDamageState;
	friend class UPlayerShadowState;
	friend class UPlayerDeadState;
	friend class UPlayerInteractState;
	friend class UPlayerInteractAnimationState;

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
	const FCameraViewSetting& GetCameraViewSetting() const;

private:

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
	UPlayerStateBase* GetPlayerState() const;

	//現在ステートのAnimBP用の状態取得
	UFUNCTION(BlueprintPure, Category = "state")
	EPlayerStatus GetPlayerStatus() const;

	UFUNCTION(BlueprintPure, Category = "state")
	UPlayerStateMachine* GetPlayerStateMachine() const;

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsCrouch() const;

	UFUNCTION(BlueprintPure, Category = "state")
	bool CanAttack() const;

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsShadow() const;

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsDash() const;

	UFUNCTION(BlueprintPure, Category = "state")
	bool IsInvincible() const;

	UFUNCTION(BlueprintCallable,Category = "Animation")
	void OnInteractAnimationEnd();

	bool CanReceivePlayerInput() const;

	//HP取得
	int32 GetPlayerHP() const { return m_playerInfo.hp; }

protected:
	//---入力用---
	// Enhanced Input 関数
	void Enhanced_Move(const FInputActionValue& Value);		//移動	
	void Enhanced_EndMove(const FInputActionValue& Value);
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
	float GetShadowTimer() const;

	//最大影潜り時間取得
	float GetMaxShadowTime() const;

private:
	bool IsUpPosWall(const FVector& _startPos)const;

private:

	UPROPERTY(EditDefaultsOnly, Category = "State Machine", meta = (DisplayName = "Player State Machine Class"))
	TSubclassOf<UPlayerStateMachine> m_stateMachineClass;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerStateMachine> m_stateMachine;

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

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMesh* m_defaultMesh;					//通常のメッシュ

	UPROPERTY(EditAnywhere, Category = "Collision")
	UCapsuleComponent* m_Capsule;						//カプセルコンポーネント

	UPROPERTY(EditAnywhere, Category = "Collision")
	float m_capsuleRadius;							//カプセル半径

	UPROPERTY(EditAnywhere, Category = "Collision")
	float m_capsuleHeight;							//カプセル高さ

	//カメラフォーカス用変数
	UPROPERTY(Transient)
	AActor* m_pSaveCameraActor;//フォーカス前のカメラアクター保存用

	UPROPERTY(EditAnywhere, Category = "CameraFocus")
	UCameraFocusDirectorComponent* m_pCameraFocusDirector;//カメラフォーカスディレクターコンポーネント

	//UPROPERTY(EditAnywhere, Category = "weapon");
	//USwordAttackComponent* m_pSword;			//ソード攻撃コンポーネント
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USwordAttackComponent* m_sword;	//ソード攻撃コンポーネント

	bool m_bCameraSwitching;						//カメラ視点切り替え中か？
	bool m_bCanControl;								//操作可能状態か

	UPROPERTY(VisibleAnywhere, Category = "Sahdow")
	bool m_bOnShadow;                               //影の上にいるか

	FVector2D m_charaMoveInput;						//キャラ移動入力量
	FVector2D m_cameraRotateInput;					//カメラ回転量

	UPROPERTY(EditAnywhere, Category = "Camera")
	FCameraViewSetting ThirdPersonCamera;

	UPROPERTY(EditAnywhere, Category = "Camera")
	FCameraViewSetting TopDownCamera;

	UPROPERTY(Transient)
	FCameraViewSetting EventCamera;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float m_maxOffsetY;	//オフセットY座標の最大値

	float m_OutcameraInputTimer;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float m_initOffsetY;

	UPROPERTY(Transient)
	TArray <AActor*> m_hitActors;//衝突対処のアドレス

	//インタラクト可なオブジェクト操作範囲内か
	bool m_bHitIntteractObject;
	//インタラクトするオブジェクト
	UPROPERTY(Transient)
	AInteract* m_hitInteractOb;

	
	
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	AEnemyBase* GetNearestEnemy();

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



};
