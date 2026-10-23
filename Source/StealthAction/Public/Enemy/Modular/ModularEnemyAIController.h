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
	AModularEnemyAIController();
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
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UAIPerceptionComponent> EnemyPerception;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|AI")
	TObjectPtr<UBehaviorTreeComponent> EnemyBehaviorTree;

private:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	bool ValidateBlackboard() const;
	void TryStartPendingTree(float DeltaTime);
	void ClearCombatTarget();
	UPROPERTY(Transient)
	TObjectPtr<UModularEnemyStateMachine> StateMachine = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTree> PendingTree = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTree> ActiveTree = nullptr;
	TWeakObjectPtr<AActor> CombatTarget;
	float TimeWithoutSight = 0.f;
	float AbortWaitTime = 0.f;
	bool bTargetVisible = false;
	bool bSwitchPending = false;
	bool bAbortWarningShown = false;
	bool bInitialized = false;
};
