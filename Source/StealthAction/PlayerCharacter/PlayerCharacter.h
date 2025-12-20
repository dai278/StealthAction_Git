//----------------------------------------------------------
// 概要				：プレイヤーキャラを制御するCharacterオブジェクト
// 更新日　　　　 　：11/19　
//----------------------------------------------------------

//BPで使えるかは日髙の独断と偏見で決めるので後で必要に応じて変更して良し
//カメラと平行移動の最低限くらいしか書きません


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
class AEnemy_1;//エネミーの基底クラスにする日髙変更


//パラメーター更新が起きた時のイベントディスパッチャー宣言
//(BPとやり取りあり、引数なし）
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateParamEventDispatcher);


//プレイヤーのカメラ視点状態
//enum classはクラス外に書かないといけないらしいし、この書き方が良いらしい
//暗殺シーンとかカメラ変えるかもしれないから今は2種しかないけどenumで
UENUM(BlueprintType)
enum class ECameraStatus : uint8
{
	ThirdPerson      UMETA(DisplayName = "Third Person"),//三人称
	TopDownView      UMETA(DisplayName = "Top Down View"),//俯瞰
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

UCLASS()
class STEALTHACTION_API APlayerCharacter : public ACharacter
{
	//カメラ視点設定構造体
	struct FCameraViewSetting
	{
		float yaw;
		float pitch;
		float roll;
		float springArmLength;

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


private:
	//カメラ更新
	void UpdateCamera(float _deltaTime);

	//カメラコリジョン判定処理
	void UpdateCameraCollision();

	//カメラ視点切り替え
	void ViewpointSwitching(float _deltaTime);

private:
	//アイドル状態の更新処理
	void UpdateIdle(float _deltaTime);

	//歩き状態の更新処理
	void UpdateMove(float _deltaTime);

	//しゃがみ状態の更新処理
	void UpdateCrouch(float _deltaTime);

	//攻撃状態の更新処理
	void UpdateAttack(float _deltaTime = 0);

	//ダメージ状態の更新処理
	void UpdateDamaged(float _deltaTime);

	//やられ状態の更新処理
	void UpdateDead(float _deltaTime);

	//影状態の更新処理
	void UpdateShadow(float _deltaTime);

	void TransformationShadowToIdle(bool flag = false);

protected:
	//---入力用---
	// Enhanced Input 関数
	void Enhanced_MoveVertical(const FInputActionValue& Value);		//X軸移動
	void Enhanced_MoveLateral(const FInputActionValue& Value);		//Y軸移動
	void Enhanced_MoveDash(const FInputActionValue& Value);			//ダッシュ
	void Enhanced_MoveCrouch(const FInputActionValue& Value);		//しゃがみ
	void Enhanced_MoveJump(const FInputActionValue& Value);			//ジャンプ
	void Enhanced_Attack(const FInputActionValue& Value);			//攻撃
	void Enhanced_InShadow(const FInputActionValue& Value);			//影潜り
	void Enhanced_changeWeapon(const FInputActionValue& Value);		//武器変更

	void Enhanced_CameraPitch(const FInputActionValue& Value);		//カメラピッチ
	void Enhanced_CameraYaw(const FInputActionValue& Value);		//カメラヨー
	void Enhanced_CameraReset(const FInputActionValue& Value);		//カメラリセット
	void Enhanced_CameraSwitch(const FInputActionValue& Value);		//視点変更

	//離された瞬間値を０にするよう
	void Enhanced_MoveVerticalReleased(const FInputActionValue& Value);	//X軸移動
	void Enhanced_MoveLateralReleased(const FInputActionValue& Value);	//Y軸移動

	void Enhanced_CameraPitchReleased(const FInputActionValue& Value);	//カメラピッチ
	void Enhanced_CameraYawReleased(const FInputActionValue& Value);	//カメラヨー

protected:
	// デフォルトで使う Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	//移動Y軸
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveVerticalIA;

	//移動X軸
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* m_moveLateralIA;

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

public:
	//影状態か？
	bool IsInShadow()const;

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
	float m_cameRotateSpeed;						//カメラ回転速度

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
	float m_timer;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRange;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRadius;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamage;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackCooldown;

	FTimerHandle AttackCooldownHandle;

	bool m_bCameraSwitching;						//カメラ視点切り替え中か？
	bool m_bCanControl;								//操作可能状態か

	bool m_bOnShadow;                               //影の上にいるか

	bool m_bUsingMesh;								//使ってるメッシュを管理するフラグ
	
	bool m_bIsCrouch;

	bool bCanAttack;

	FVector2D m_charaMoveInput;						//キャラ移動入力量
	FVector2D m_cameraRotateInput;					//カメラ回転量

	FCameraViewSetting m_cameraInitPos[(int)ECameraStatus::Num];	//各視点変更時最初の視点

	TArray <AActor*> m_hitActors;//衝突対処のアドレス

	//日髙変更点
	//隠密キル対象のアドレス
	AEnemy_1* m_pStealthKillEnemy;
	
	//デバック用
	UNoiseListenerComponent* noise;
	public:
	void OnNoiseHeard(const int& _noiseVolume, const FVector& _pos);

};
