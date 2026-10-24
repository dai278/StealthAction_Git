//----------------------------------------------------------
//概要：新しい敵のAI制御
//----------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ModularEnemyAIController.generated.h"

class AModularEnemyCharacter;
class UModularEnemyStateMachine;
class UBehaviorTree;
class UBehaviorTreeComponent;
class UBlackboardData;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

//BTの実行はここだけで管理する
UCLASS(Blueprintable)

class STEALTHACTION_API AModularEnemyAIController : public AAIController
{
	GENERATED_BODY()

  public:

	//コンストラクタ
	AModularEnemyAIController();

	//更新処理
	virtual void Tick(float DeltaTime) override;

	//PawnのBeginPlayとPossessの順番に依存しない初期化
	void InitializeEnemy(AModularEnemyCharacter* Enemy);

	//前のBTの中断を待ってから切り替える
	void RequestBehaviorTree(UBehaviorTree* Tree);

	//BTの停止(終了時だけ強制停止)
	void StopEnemyLogic(bool bForce = false);

	//覚えている相手がいるか
	bool HasCombatTarget() const;

	//今見えている相手だけ取得
	AActor* GetVisibleTarget() const;

	//現在の敵ステートマシン取得
	UFUNCTION(BlueprintPure, Category = "Enemy")

	UModularEnemyStateMachine* GetEnemyStateMachine() const { return StateMachine; }

	//調査完了時のBlackboard更新
	void FinishInvestigation();

	//巡回先のBlackboard更新
	bool SetNextPatrolLocation();

	//物音などの調査先(既存の独自NoiseManagerから呼ぶ場合もここを使う)
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void InvestigateLocation(FVector Location);

  protected:

	//敵の操作開始
	virtual void OnPossess(APawn* InPawn) override;

	//敵の操作終了
	virtual void OnUnPossess() override;

	//終了処理
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

	//探索BTと戦闘BTで同じBlackboardを使用する
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UBlackboardData> SharedBlackboard = nullptr;

	//空ならC++の4ステートを使用する
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|AI")
	TSubclassOf<UModularEnemyStateMachine> StateMachineClass;

	//見失ってから探索へ戻るまでの時間
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|AI", meta = (ClampMin = "0.0"))
	float LoseSightDelay = 5.f;

	//独自のタスクが中断を完了しない場合の警告時間(強制切り替えはしない)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|AI", meta = (ClampMin = "0.1"))
	float AbortWarningDelay = 2.f;

	//敵の感知コンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UAIPerceptionComponent> EnemyPerception;

	//視覚の設定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	//聴覚の設定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	//BTの実行コンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UBehaviorTreeComponent> EnemyBehaviorTree;

  private:

	//感知結果の更新
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	//Blackboardの設定確認
	bool ValidateBlackboard() const;

	//中断完了を待ってBT開始
	void TryStartPendingTree(float DeltaTime);

	//戦闘対象の解除
	void ClearCombatTarget();

	//敵のステートマシン
	UPROPERTY(Transient)
	TObjectPtr<UModularEnemyStateMachine> StateMachine = nullptr;

	//中断後に開始するBT
	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTree> PendingTree = nullptr;

	//現在実行しているBT
	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTree> ActiveTree = nullptr;

	//覚えている戦闘対象
	TWeakObjectPtr<AActor> CombatTarget;

	//対象を見失ってからの時間
	float TimeWithoutSight = 0.f;

	//BTの中断を待っている時間
	float AbortWaitTime = 0.f;

	//対象が見えているか
	bool bTargetVisible = false;

	//BTの切り替え待ちか
	bool bSwitchPending = false;

	//中断待ちの警告を表示済みか
	bool bAbortWarningShown = false;

	//初期化済みか
	bool bInitialized = false;
};
