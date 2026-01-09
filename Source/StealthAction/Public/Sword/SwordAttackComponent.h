//--------------------------------------------------------
//ソードの攻撃コンポーネント(攻撃判定のみ指定の位置に置く)
//--------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"  
#include "SwordAttackComponent.generated.h"

class USphereComponent;

//クラスのメンバ関数をノイズイベントのコールバックにするときのマクロ
#define CreateSwingEndCallback(_func) std::bind(&_func, this,std::placeholders::_1)
#define CreateSwingEndCallback(_func) std::bind(&_func, this)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHACTION_API USwordAttackComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USwordAttackComponent();

protected:
	// BeginPlay 
	virtual void BeginPlay() override;
public:	
	// Tick
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// OnRegister
	virtual void OnRegister()override;
public:
	//衝突対象の追加
	void SetProfileName(const FName _name);
	//衝突対象のクリア
	void ClearCollisionObjectType();
	//剣の攻撃範囲の大きさ指定
	void SetSwordAttackScale(const float& _radius);
	//持続時間設定
	void SetAttackTime(const float& _time);
	//ノックバックの大きさ登録
	void SetKnockBackValu(const float& _knockBackValu) { m_KnockBackValu = _knockBackValu; }

	//ノックバックの大きさ取得
	float GetKnockBackValu()const { return m_KnockBackValu; }
	//ダメージ取得

	//剣を振る
	//引数：ダメージ,判定持続時間,半径,
	//攻撃二段目などに対応できるようオーバーロードも用意
	//引数無しは事前に設定した値を使う
	void Swinging(const int& _damage, const FVector& _location, const float& _radius, const float& _attackTime, const bool& _bSneakKill);
	void Swinging(const bool& _bSneakKill);
	
	//任意のタイミングで降り終わらせられるようにする
	//ダメージくらった時など
	void SwingEnd();

	//剣の攻撃判定用コリジョンと衝突した瞬間のイベント関数
	UFUNCTION()
	void OnSwordBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
		
	//剣を振り終わったこと通知用コールバック関数登録
	void RegisterSwingEndCallback(const std::function<void()>& func);

private:
	//剣を振り終わったこと通知用コールバック関数
	std::function<void()> m_swingEndCallback;

	UPROPERTY(VisibleAnywhere, Category = "SwordCollision")
	USphereComponent* m_swordCollision;	//剣の攻撃判定用コリジョン

	int m_damage;						//与えるダメージ
	float m_radius;						//攻撃範囲の大きさ
	float m_timer;						//持続時間タイマー
	float m_swingEndTime;				//剣を振り終わる時間
	bool m_bSneakKill;					//スニークキルかどうか
	bool m_bIsSwinging;                 //剣を振っている最中かどうか
	float m_KnockBackValu;              //ノックバックの大きさ
};
