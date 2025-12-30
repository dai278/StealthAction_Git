//----------------------------------------------------------
// 概要				：エネミー1を制御するオブジェクト
// 更新日　　　　 　：
// 担当				：24CU0237廣川菖
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy_1.generated.h"

// 前方宣言
class APlayerCharacter;
class UNoiseListenerComponent;
class UShadowComponent;
class AEnemy_Bullet_1;
class AEnemy_Weapon_1;
class AExtendedSpotLight;


//エネミーの状態
UENUM(BlueprintType)
enum class EEnemy_1Status :uint8
{
	Empty			UMETA(DisplayName = "Empty"),

	Patrol			UMETA(DisplayName = "Patrol"),
	Miss			UMETA(DisplayName = "Miss"),
	Return			UMETA(DisplayName = "Return"),
	Dead			UMETA(DisplayName = "Dead"),

	Doubt			UMETA(DisplayName = "Doubt"),
	Caution			UMETA(DisplayName = "Caution"),
	Battle			UMETA(DisplayName = "Battle"),

	Doubt_Noise			UMETA(DisplayName = "Doubt_Noise"),
	Caution_Noise		UMETA(DisplayName = "Caution_Noise"),
	Battle_Noise			UMETA(DisplayName = "Battle_Noise")

};

UCLASS()
class STEALTHACTION_API AEnemy_1 : public ACharacter
{

	//エネミー情報格納用構造体
	//struct Enemy_1_INFO
	//{
	//	int hp;

	//	Enemy_1_INFO() :hp(100) {}
	//};

	GENERATED_BODY()

public:
	// コンストラクタ
	AEnemy_1();

protected:
	// ゲームスタート時、または生成時に呼ばれる処理
	virtual void BeginPlay() override;

public:
	// 毎フレームの更新処理
	virtual void Tick(float DeltaTime) override;

	// 各入力関係メソッドとのバインド処理
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	//視界処理
	void UpdateVisiblity(float _deltaTime);

	//聴覚処理
	void UpdateHearing(float _deltaTime);

	//探索処理
	void UpdateSearch(float _deltaTime);

	//ステータス処理
	void UpdateStatus(float _deltaTime);

	//巡回の処理
	void CasePatrol(float _deltaTime);

	//疑念の処理
	void CaseDoubt(float _deltaTime);

	//注意の処理
	void CaseCaution(float _deltaTime);

	//戦闘の処理
	void CaseBattle(float _deltaTime);

	//失踪の処理
	void CaseMiss(float _deltaTime);

	//帰還の処理
	void CaseReturn(float _deltaTime);

	//死亡の処理
	void CaseDead(float _deltaTime);

	//物音疑念の処理
	void CaseDoubt_Noise(float _deltaTime);

	//物音注意の処理
	void CaseCaution_Noise(float _deltaTime);

	//物音戦闘の処理
	void CaseBattle_Noise(float _deltaTime);

	//視点移動の処理
	void UpdateViewMove(float _deltaTime);

	//移動の処理
	void UpdateMove(float _deltaTime);

	//移動(Nav)の処理
	void UpdateMove_Nav(float _deltaTime);

	//警戒の処理
	void UpdateAlert(float _deltaTime);

	//初めの処理
	void StartStateValues(float _deltaTime);

	//リセットの処理
	void ResetStateValues(float _deltaTime);

	//攻撃の処理
	void UpdateAttack(float _deltaTime);

	//攻撃を受けたときの処理
	void OnDamage(const int& _damage,const FVector& _knockBackVector, const bool& _bSneakKill);

	//衝突判定
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	//プレイヤーを見つけているかどうか
	//確認が取れてないので仮でfalse返す
	//確認取れたらCpp側で実装予定
	bool IsPlayerFound() const { return m_battleCheck; }

public:

private:


	UPROPERTY()
	APlayerCharacter* m_pPlayerChara;	//プレイヤーキャラクターポインタ

	TArray<AEnemy_1*> m_pOtherEnemy_1;	//その他のエネミー_1ポインタ

	UPROPERTY(EditAnywhere, Category = "Weapon")
	AEnemy_Weapon_1* m_pEnemy_Weapon;			//銃ポインタ

	UPROPERTY()
	TArray<AEnemy_Bullet_1*> m_pALLBullet_1;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UShadowComponent* m_pShadow;  //影

	//基本ステータス
	UPROPERTY(EditAnywhere, Category = "Enemy_1_Status")
	int m_HP;	//HP
	//基本ステータス
	UPROPERTY(EditAnywhere, Category = "Enemy_1_Status")
	float m_hitDamage;			//ダメージ量


	//視界用
	UPROPERTY(EditAnywhere, Category = "Visiblity")
	float m_visiblityAngle;			//視野角
	UPROPERTY(EditAnywhere, Category = "Visiblity")
	double m_visionRange_Short;		//視界範囲(近い)
	UPROPERTY(EditAnywhere, Category = "Visiblity")
	double m_visionRange_Normal;	//視界範囲(普)
	UPROPERTY(EditAnywhere, Category = "Visiblity")
	double m_visionRange_Long;		//視界範囲(遠い)

	int m_visionLevel;				//視界レベル
	bool m_visionCheck;				//視界結果


	//聴覚用
	UPROPERTY(EditAnywhere, Category = "Hearing")
	double m_hearingRange_Short;	//聴覚範囲(良)
	UPROPERTY(EditAnywhere, Category = "Hearing")
	double m_hearingRange_Normal;	//聴覚範囲(普)
	UPROPERTY(EditAnywhere, Category = "Hearing")
	double m_hearingRange_Long;	//聴覚範囲(不)

	UPROPERTY(EditAnywhere, Category = "Hearing")
	double m_stopDistance_Noise;		//音に近づける限界距離

	bool m_noiseCheck;					//物音チェック
	FVector m_noise_Pos;				//物音の座標
	FVector m_noise_Pos_keeper;			//一個前の物音の座標
	int m_noiseLevel;					//騒音取得用
	int m_noiseLevel_keeper;			//一個前の物音レベル


	//時間関連
	UPROPERTY(EditAnywhere, Category = "Time")
	double m_patrolTime_Limit;	//巡回時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_patrol_TurningTime_Limit;			//巡回旋回時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_patrol_TurningCheckingTime_Limit;	//巡回旋回時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_doubtTime_Limit;			//疑念時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_doubtNoiseTime_Limit;			//疑念時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_cautionTime_Limit;			//注意時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_cautionNoiseTime_Limit;			//注意時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_battleTime_Limit;			//戦闘時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_battleFalseTime_Limit;			//戦闘時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_battleNoiseTime_Limit;			//戦闘時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_alertTime_Limit;			//警戒時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_missTime_Limit;			//失踪時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_returnTime_Limit;			//帰還時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_hearingTime_Limit;			//聴覚時間制限

	UPROPERTY(EditAnywhere, Category = "Time")
	double m_attackingTime_Limit;			//攻撃時間制限


	double m_patrolTime;				//巡回時間
	double m_patrol_TurningCheckingTime;			//旋回している時間
	double m_doubtTime;					//疑念時間
	double m_doubtNoiseTime;			//物音疑念時間
	double m_cautionTime;				//注意時間
	double m_cautionNoiseTime;			//物音注意時間
	double m_battleTime;				//戦闘時間
	double m_battleFalseTime;			//見失い戦闘時間
	double m_battleNoiseTime;			//物音戦闘時間
	double m_alertTime;					//警戒時間
	double m_missTime;					//失踪時間
	double m_returnTime;				//帰還時間
	double m_hearingTime;				//聴覚時間
	double m_attackingTime;				//攻撃時間

	//移動関連
	UPROPERTY(EditAnywhere, Category = "Chase")
	float m_chaseSpeed_Slow;			//追跡速度(遅い)
	UPROPERTY(EditAnywhere, Category = "Chase")
	float m_chaseSpeed_Normal;			//追跡速度(普通)
	UPROPERTY(EditAnywhere, Category = "Chase")
	float m_chaseSpeed_Fast;			//追跡速度(早い)

	UPROPERTY(EditAnywhere, Category = "Chase")
	float m_chaseRotSpeed;		//追跡中キャラ回転速度
	UPROPERTY(EditAnywhere, Category = "Chase")
	double m_stopDistance_Player;		//プレイヤーに近づける限界距離
	UPROPERTY(EditAnywhere, Category = "Chase")
	double m_stopDistance_2D;		//プレイヤーに近づける限界距離（2D）
	UPROPERTY(EditAnywhere, Category = "Chase")
	double m_stopDistance_Nav;		//プレイヤーに近づける限界距離（Nav）
	UPROPERTY(EditAnywhere, Category = "Chase")
	double m_attackDistance;	//攻撃範囲

	float m_currentChaseSpeed;			//現在の追跡速度

	bool m_moveStop_Nav;		//追跡停止フラグ（Nav）


	//状態関連
	bool m_alertCheck;			//警戒状態か
	bool m_patrolCheck;			//巡回状態か
	bool m_doubtCheck;			//疑念状態か
	bool m_doubtNoiseCheck;		//物音疑念状態か
	bool m_cautionCheck;		//注意状態か
	bool m_cautionNoiseCheck;	//物音注意状態か
	bool m_battleCheck;			//戦闘状態か
	bool m_battleNoiseCheck;	//物音戦闘状態か
	bool m_missCheck;			//失踪状態か
	bool m_returnCheck;			//帰還状態か

	bool m_searchStopper;		//視界判定を停止するか
	bool m_hearingStopper;		//聴覚判定を停止するか

	bool m_playerShadowCheck;		//プレイヤーが影状態か？

	bool m_deadCheck;			//死亡したか

	//ステータス関連
	EEnemy_1Status m_enemyCurrentState; //エネミーのステータス
	EEnemy_1Status m_enemyCurrentState_Keeper; //一つ前のステータス(1F)
	EEnemy_1Status m_enemyCurrentState_Check; //チェック用のステータス(1F)


	//座標関連
	FVector m_enemyPos;				//エネミーの座標
	FVector m_enemyForward;			//エネミーの正面ベクトル
	FVector m_enemyRight;			//エネミーの右ベクトル
	FVector m_playerPos;				//プレイヤーの座標
	FVector m_playerPos_LastSeen;		//見えてた時のプレイヤーの座標
	FVector m_playerPos_Nav_LastSeen;		//見えてた時のプレイヤーの座標（Nav）

	//ルート関係
	UPROPERTY(EditAnywhere, Category = "Route")
	bool m_randomRoute;			//ランダムパトロールか？
	UPROPERTY(EditAnywhere, Category = "Route")
	int m_routeNum;			//ルート番号
	FVector m_routePos;			//ルートの座標か？
	int m_routeCounter;			//進行中のルート番号

	TArray<AActor*> m_pEnemy_Route;	//エネミーのルートポインタ


	//巡回関係
	UPROPERTY(EditAnywhere, Category = "Patrol")
	bool m_patrolCancel;			//巡回中動くか？

	bool	m_patrol_TurningCheck;			//旋回しているか
	int	m_patrol_TurnDirection;				//旋回方向の決定
	UPROPERTY(EditAnywhere, Category = "Patrol")
	double m_stopDistance_Wall;				//壁に近づける距離

	FVector m_enemyPos_XY_Wall[4];				//エネミーの上下左右の座標
	FVector m_turnDirection_Pos;				//エネミーの上下左右の座標
	FVector m_turnRight_Pos;				//エネミーの上下左右の座標


	//見失う関係
	FVector m_enemyPos_Forward_Miss;		//エネミーの正面の座標
	FVector m_enemyPos_Left_Miss;		//エネミー座標から左座標
	FVector m_enemyPos_Right_Miss;		//エネミー座標から右座標
	FVector m_enemyPos_Direction_Miss;		//エネミー座標から右座標


	//帰還関係
	FVector m_enemyPos_Return;				//エネミーの元居たの座標
	FVector m_enemyDirection_Return;		//エネミーの元居たの向き

	float m_allTime;


	// ライトSpawn  Attach
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AExtendedSpotLight> m_spotLightClass;   // 生成に使うライトBPクラス

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AExtendedSpotLight> m_spotLightInstance; // 生成されたライト実体

	UNoiseListenerComponent* noise;
	void OnNoiseHeard(const int& _noiseVolume, const FVector& _pos);
};