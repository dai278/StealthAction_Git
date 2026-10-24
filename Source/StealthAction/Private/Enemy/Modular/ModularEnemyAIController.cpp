//----------------------------------------------------------
//概要：新しい敵のAI制御
//----------------------------------------------------------

#include "Enemy/Modular/ModularEnemyAIController.h"
#include "Enemy/Modular/ModularEnemyCharacter.h"
#include "Enemy/Modular/ModularEnemyStateMachine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
AModularEnemyAIController::AModularEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	StateMachineClass = UModularEnemyStateMachine::StaticClass();
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("EnemyBlackboard"));
	EnemyBehaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("EnemyBehaviorTree"));
	BrainComponent = EnemyBehaviorTree;
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerception"));
	SetPerceptionComponent(*EnemyPerception);
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1800.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	EnemyPerception->ConfigureSense(*SightConfig);
	EnemyPerception->ConfigureSense(*HearingConfig);
	EnemyPerception->SetDominantSense(UAISense_Sight::StaticClass());
	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AModularEnemyAIController::OnTargetPerceptionUpdated);
}

//-----------------------------------------------------
//敵の操作開始
//-----------------------------------------------------
void AModularEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AModularEnemyCharacter* Enemy = Cast<AModularEnemyCharacter>(InPawn);
	if (Enemy && Enemy->HasActorBegunPlay())
	{
		InitializeEnemy(Enemy);
	}
}

//-----------------------------------------------------
//初期化
//-----------------------------------------------------
void AModularEnemyAIController::InitializeEnemy(AModularEnemyCharacter* Enemy)
{
	if (bInitialized || !IsValid(Enemy) || GetPawn() != Enemy)
	{
		return;
	}
	if (!IsValid(SharedBlackboard) || !StateMachineClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Set Shared Blackboard and State Machine Class."), *GetName());
		return;
	}
	UBlackboardComponent* BB = nullptr;
	if (!UseBlackboard(SharedBlackboard.Get(), BB) || !ValidateBlackboard())
	{
		return;
	}
	BB->SetValueAsVector(TEXT("HomeLocation"), Enemy->GetActorLocation());
	BB->SetValueAsVector(TEXT("PatrolLocation"), Enemy->GetActorLocation());
	BB->SetValueAsBool(TEXT("CanSeeTarget"), false);
	BB->SetValueAsBool(TEXT("HasInvestigation"), false);
	BB->ClearValue(TEXT("TargetActor"));
	bInitialized = true;
	StateMachine = NewObject<UModularEnemyStateMachine>(this, StateMachineClass);
	StateMachine->Initialize(Enemy);
	if (!StateMachine->IsConfigured())
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Enemy state configuration failed."), *GetName());
		StopEnemyLogic(true);
		bInitialized = false;
	}
}

//-----------------------------------------------------
//Blackboardのキー名と型を確認
//-----------------------------------------------------
bool AModularEnemyAIController::ValidateBlackboard() const
{
	const UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		return false;
	}
	const FName VectorKeys[] = {TEXT("HomeLocation"), TEXT("PatrolLocation"), TEXT("LastKnownLocation"), TEXT("InvestigationLocation")};
	const FName BoolKeys[] = {TEXT("CanSeeTarget"), TEXT("HasInvestigation")};
	bool bValid = true;
	auto Check = [&](FName Key, UClass* Type)
	{
		const FBlackboard::FKey ID = BB->GetKeyID(Key);
		if (ID == FBlackboard::InvalidKey || BB->GetKeyType(ID) != Type)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Blackboard key '%s' is missing or has the wrong type."), *GetName(), *Key.ToString());
			bValid = false;
		}
	};
	Check(TEXT("TargetActor"), UBlackboardKeyType_Object::StaticClass());
	for (FName Key : VectorKeys)
	{
		Check(Key, UBlackboardKeyType_Vector::StaticClass());
	}
	for (FName Key : BoolKeys)
	{
		Check(Key, UBlackboardKeyType_Bool::StaticClass());
	}
	return bValid;
}

//-----------------------------------------------------
//BTの切り替え要求
//-----------------------------------------------------
void AModularEnemyAIController::RequestBehaviorTree(UBehaviorTree* Tree)
{
	if (!bInitialized || !IsValid(EnemyBehaviorTree))
	{
		return;
	}

	//同じBTを毎フレーム再起動しない
	if ((bSwitchPending && PendingTree == Tree) || (!bSwitchPending && ActiveTree == Tree && EnemyBehaviorTree->IsRunning()))
	{
		return;
	}
	StopMovement();
	EnemyBehaviorTree->StopTree(EBTStopMode::Safe);
	ActiveTree = nullptr;
	PendingTree = Tree;
	bSwitchPending = IsValid(Tree);
	AbortWaitTime = 0.f;
	bAbortWarningShown = false;
}

//-----------------------------------------------------
//中断完了を待って次のBTを開始
//-----------------------------------------------------
void AModularEnemyAIController::TryStartPendingTree(float DeltaTime)
{
	if (!bSwitchPending || !IsValid(EnemyBehaviorTree))
	{
		return;
	}
	if (EnemyBehaviorTree->IsAbortPending() || EnemyBehaviorTree->TreeHasBeenStarted())
	{
		AbortWaitTime += FMath::Max(0.f, DeltaTime);
		if (!bAbortWarningShown && AbortWaitTime >= FMath::Max(0.1f, AbortWarningDelay))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: Waiting for BT abort. Check ReceiveAbort/FinishAbort and latent task cleanup."), *GetName());
			bAbortWarningShown = true;
		}
		return;
	}
	UBehaviorTree* Next = PendingTree.Get();
	PendingTree = nullptr;
	bSwitchPending = false;
	if (!IsValid(Next))
	{
		return;
	}

	//異なるBlackboardへの切り替えは許可しない
	if (Next->BlackboardAsset != SharedBlackboard)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Both behavior trees must use the exact Shared Blackboard asset."), *GetName());
		return;
	}
	if (EnemyBehaviorTree->IsPaused())
	{
		EnemyBehaviorTree->ResumeLogic(TEXT("Enemy state transition"));
	}
	ActiveTree = Next;
	EnemyBehaviorTree->StartTree(*Next, EBTExecutionMode::Looped);
}

//-----------------------------------------------------
//BTと移動の停止
//-----------------------------------------------------
void AModularEnemyAIController::StopEnemyLogic(bool bForce)
{
	PendingTree = nullptr;
	ActiveTree = nullptr;
	bSwitchPending = false;
	if (IsValid(EnemyBehaviorTree))
	{
		EnemyBehaviorTree->StopTree(bForce ? EBTStopMode::Forced : EBTStopMode::Safe);
	}
	StopMovement();
	ClearFocus(EAIFocusPriority::Gameplay);
}

//-----------------------------------------------------
//視覚と聴覚の通知
//-----------------------------------------------------
void AModularEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	const AModularEnemyCharacter* Enemy = Cast<AModularEnemyCharacter>(GetPawn());
	const APawn* SensedPawn = Cast<APawn>(Actor);
	if (!bInitialized || !Enemy || !Enemy->IsAlive() || !IsValid(Actor))
	{
		return;
	}
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB)
	{
		return;
	}
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			InvestigateLocation(Stimulus.StimulusLocation);
		}
		return;
	}
	if (Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>() || !SensedPawn || !SensedPawn->IsPlayerControlled())
	{
		return;
	}
	if (Stimulus.WasSuccessfullySensed())
	{
		CombatTarget = Actor;
		bTargetVisible = true;
		TimeWithoutSight = 0.f;
		BB->SetValueAsObject(TEXT("TargetActor"), Actor);
		BB->SetValueAsVector(TEXT("LastKnownLocation"), Actor->GetActorLocation());
		BB->SetValueAsBool(TEXT("CanSeeTarget"), true);
	}
	else if (CombatTarget.Get() == Actor)
	{
		bTargetVisible = false;
		TimeWithoutSight = 0.f;
		BB->ClearValue(TEXT("TargetActor"));
		BB->SetValueAsBool(TEXT("CanSeeTarget"), false);
	}
}

//-----------------------------------------------------
//戦闘対象を覚えているか
//-----------------------------------------------------
bool AModularEnemyAIController::HasCombatTarget() const
{
	return CombatTarget.IsValid();
}

//-----------------------------------------------------
//今見えている対象を取得
//-----------------------------------------------------
AActor* AModularEnemyAIController::GetVisibleTarget() const
{
	return bTargetVisible ? CombatTarget.Get() : nullptr;
}

//-----------------------------------------------------
//戦闘対象の解除
//-----------------------------------------------------
void AModularEnemyAIController::ClearCombatTarget()
{
	CombatTarget.Reset();
	bTargetVisible = false;
	TimeWithoutSight = 0.f;
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->ClearValue(TEXT("TargetActor"));
		BB->SetValueAsBool(TEXT("CanSeeTarget"), false);
	}
}

//-----------------------------------------------------
//調査先の設定
//-----------------------------------------------------
void AModularEnemyAIController::InvestigateLocation(FVector Location)
{
	const AModularEnemyCharacter* Enemy = Cast<AModularEnemyCharacter>(GetPawn());
	if (!bInitialized || !Enemy || !Enemy->IsAlive() || HasCombatTarget())
	{
		return;
	}
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsVector(TEXT("InvestigationLocation"), Location);
		BB->SetValueAsBool(TEXT("HasInvestigation"), true);
	}
}

//-----------------------------------------------------
//調査完了
//-----------------------------------------------------
void AModularEnemyAIController::FinishInvestigation()
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsBool(TEXT("HasInvestigation"), false);
	}
}

//-----------------------------------------------------
//次の巡回地点をBlackboardへ設定
//-----------------------------------------------------
bool AModularEnemyAIController::SetNextPatrolLocation()
{
	AModularEnemyCharacter* Enemy = Cast<AModularEnemyCharacter>(GetPawn());
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!Enemy || !BB || !Enemy->IsAlive())
	{
		return false;
	}
	BB->SetValueAsVector(TEXT("PatrolLocation"), Enemy->GetNextPatrolLocation());
	return true;
}

//-----------------------------------------------------
//感知情報更新→ステート更新→BT開始の順で処理
//-----------------------------------------------------
void AModularEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bInitialized)
	{
		return;
	}
	if (CombatTarget.IsValid())
	{
		if (bTargetVisible)
		{
			if (UBlackboardComponent* BB = GetBlackboardComponent())
			{
				BB->SetValueAsVector(TEXT("LastKnownLocation"), CombatTarget->GetActorLocation());
			}
		}
		else
		{
			TimeWithoutSight += FMath::Max(0.f, DeltaTime);
			if (TimeWithoutSight >= FMath::Max(0.f, LoseSightDelay))
			{
				const UBlackboardComponent* BB = GetBlackboardComponent();
				const FVector LastLocation = BB ? BB->GetValueAsVector(TEXT("LastKnownLocation")) : FVector::ZeroVector;
				ClearCombatTarget();
				InvestigateLocation(LastLocation);
			}
		}
	}
	else if (bTargetVisible)
	{
		ClearCombatTarget();
	}
	if (IsValid(StateMachine))
	{
		StateMachine->Update(DeltaTime);
	}
	TryStartPendingTree(DeltaTime);
}

//-----------------------------------------------------
//敵の操作終了
//-----------------------------------------------------
void AModularEnemyAIController::OnUnPossess()
{
	if (IsValid(StateMachine))
	{
		StateMachine->Stop();
	}
	StopEnemyLogic(true);
	StateMachine = nullptr;
	bInitialized = false;
	ClearCombatTarget();
	Super::OnUnPossess();
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void AModularEnemyAIController::EndPlay(const EEndPlayReason::Type Reason)
{
	if (IsValid(StateMachine))
	{
		StateMachine->Stop();
	}
	StopEnemyLogic(true);
	Super::EndPlay(Reason);
}
