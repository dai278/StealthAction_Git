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
};


//パラメーター更新が起きた時のイベントディスパッチャー宣言
//(BPとやり取りあり、引数なし）
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateParamEventDispatcher);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerConditionMet, EPlayerStatus, ConditionId);


UCLASS()
class STEALTHACTION_API APlayerCharacter : public ACharacter,public IDamageable
{
	//カメラ視点設定構造体
	struct FCameraViewSetting
	{
		FRotator rotator;//回転
		float fieldOfView;//視野角
		float springArmLength;//長さ
	};

	GENERATED_BODY()

public:
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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerConditionMet OnPlayerConditionMet;

	//ダメージ処理
	void OnDamage(int32 Damage, FVector KnockBackVec, bool bSneakKill)override;

private:
	//カメラ更新
	void UpdateCamera(float _deltaTime);

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

	//影状態の更新処理
	void UpdateShadow(float _deltaTime);

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

	//攻撃終了コールバック
	void OnAttackEnd();

	//生きているか
	UFUNCTION(BlueprintPure, Category = "Info")
	bool IsAlive() const { return m_playerInfo.isAlive; }

	//HP取得
	int32 GetPlayerHP() const { return m_playerInfo.hp; }

protected:
	//---入力用---
	// Enhanced Input 関数
	void Enhanced_Move(const FInputActionValue& Value);		//移動	
	void Enhanced_MoveDash(const FInputActionValue& Value);			//ダッシュ
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
	float GetShadowTimer() const { return m_shadowTimer; }

	//最大影潜り時間取得
	float GetMaxShadowTime() const { return m_maxShadowTime; }

	private:
	//状態変更処理
	void ChangePlayerStatus(const EPlayerStatus& _newStatus);


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

	UPROPERTY(EditAnywhere, Category = "Sahdow")
	float m_maxShadowTime;

	UPROPERTY(EditAnywhere, Category = "Sahdow")
	float m_shadowTimer;

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

	FCameraViewSetting m_cameraInitPos[(int)ECameraStatus::Num];	//各視点変更時最初の視点

	TArray <AActor*> m_hitActors;//衝突対処のアドレス

	//インタラクト可なオブジェクト操作範囲内か
	bool m_bHitIntteractObject;
	AInteract* m_hitInteractOb;

	//日髙変更点
	UEnemyManager* m_pEnemyManager;//エネミーマネージャー毎フレーム検索は重いので
	//拡張スポットライトマネージャーのアドレス
	UExtendedSpotLightManager* m_pExtendedSpotLightManager;
	AEnemyBase* m_pNearestEnemy;//一番近い敵のポインタ
	
	//デバック用
	UNoiseListenerComponent* noise;
	public:
	void OnNoiseHeard(const int& _noiseVolume, const FVector& _pos);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FPlayerInfo m_playerInfo; //プレイヤー情報構造体


};
