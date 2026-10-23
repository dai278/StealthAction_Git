#include <algorithm>
#include <cassert>
#define UE_LOG(...)
#define TEXT(x) x
namespace EBTStopMode { enum Type { Safe, Forced }; }
namespace EBTExecutionMode { enum Type { Looped }; }
namespace EAIFocusPriority { enum Type { Gameplay }; }
struct UBlackboardData {};
template<class T> struct Ptr {
    T* P = nullptr;
    Ptr(T* p = nullptr): P(p) {}
    T* Get() const { return P; }
    T* operator->() const { return P; }
    operator T*() const { return P; }
};
template<class T> bool IsValid(T* p) { return p != nullptr; }
template<class T> bool IsValid(Ptr<T> p) { return p.P != nullptr; }
struct FMath { static float Max(float a, float b) { return std::max(a, b); } };
struct UBehaviorTree { Ptr<UBlackboardData> BlackboardAsset; };
struct UBehaviorTreeComponent {
    bool Running = false, AbortPending = false, Paused = false, DelayAbort = false;
    int Starts = 0, Stops = 0;
    UBehaviorTree* Current = nullptr;
    bool IsRunning() const { return Running && !Paused; }
    bool IsAbortPending() const { return AbortPending; }
    bool TreeHasBeenStarted() const { return Running; }
    bool IsPaused() const { return Paused; }
    void ResumeLogic(const char*) { Paused = false; }
    void StopTree(EBTStopMode::Type mode) {
        ++Stops;
        if (mode == EBTStopMode::Safe && Running && DelayAbort) { AbortPending = true; }
        else { Running = AbortPending = false; Current = nullptr; }
    }
    void StartTree(UBehaviorTree& tree, EBTExecutionMode::Type) {
        assert(!AbortPending && !Running);
        ++Starts; Running = true; Current = &tree;
    }
    void FinishAbort() { Running = AbortPending = false; Current = nullptr; }
};
struct AModularEnemyAIController {
    Ptr<UBlackboardData> SharedBlackboard;
    Ptr<UBehaviorTreeComponent> EnemyBehaviorTree;
    Ptr<UBehaviorTree> PendingTree, ActiveTree;
    bool bInitialized = true, bSwitchPending = false, bAbortWarningShown = false;
    float AbortWaitTime = 0, AbortWarningDelay = 2;
    int MovementStops = 0;
    void StopMovement() { ++MovementStops; }
    void ClearFocus(EAIFocusPriority::Type) {}
    void RequestBehaviorTree(UBehaviorTree*);
    void TryStartPendingTree(float);
    void StopEnemyLogic(bool bForce = false);
};
// PRODUCTION_METHODS
int main() {
    UBlackboardData shared, wrong;
    UBehaviorTree explore{&shared}, combat{&shared}, invalid{&wrong};
    UBehaviorTreeComponent brain;
    AModularEnemyAIController ai;
    ai.SharedBlackboard = &shared; ai.EnemyBehaviorTree = &brain;
    ai.RequestBehaviorTree(&explore); ai.TryStartPendingTree(0);
    assert(brain.Starts == 1 && brain.Current == &explore);
    ai.RequestBehaviorTree(&explore); ai.TryStartPendingTree(1);
    assert(brain.Starts == 1); //同じBTは再起動しない
    brain.DelayAbort = true;
    ai.RequestBehaviorTree(&combat); ai.TryStartPendingTree(3);
    assert(brain.Starts == 1 && ai.bAbortWarningShown); //古いBTの中断待ち
    ai.RequestBehaviorTree(&combat);
    assert(ai.AbortWaitTime == 3); //同じ要求で待ち時間を消さない
    ai.RequestBehaviorTree(&explore); //待ち中に戻った場合は最新の要求を使用
    brain.FinishAbort(); ai.TryStartPendingTree(0);
    assert(brain.Starts == 2 && brain.Current == &explore);
    ai.RequestBehaviorTree(&combat); ai.StopEnemyLogic();
    brain.FinishAbort(); ai.TryStartPendingTree(0);
    assert(brain.Starts == 2 && !ai.bSwitchPending); //被ダメージ・死亡で予約を破棄
    ai.RequestBehaviorTree(&invalid); ai.TryStartPendingTree(0);
    assert(brain.Starts == 2); //Blackboard不一致は開始しない
    brain.Paused = true; ai.RequestBehaviorTree(&combat); ai.TryStartPendingTree(0);
    assert(brain.Starts == 3 && !brain.Paused); //停止済みならPauseフラグを引き継がない
    ai.StopEnemyLogic(true);
    assert(!brain.Running && !brain.AbortPending);
    ai.bInitialized = false; ai.RequestBehaviorTree(&explore);
    assert(!ai.bSwitchPending);
}
