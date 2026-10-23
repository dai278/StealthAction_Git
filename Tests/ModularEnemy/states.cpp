#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>
#define UE_LOG(...)
#define TEXT(x) x
struct UClass { const UClass* Parent = nullptr; };
struct Object { virtual ~Object() = default; const UClass* Class = nullptr;
    bool IsA(const UClass* c) const { for (auto p = Class; p; p = p->Parent) { if (p == c) return true; } return false; }
    template<class T> bool IsA() const { return IsA(T::StaticClass()); }
};
template<class T> T* Cast(Object* p) { return dynamic_cast<T*>(p); }
template<class T> bool IsValid(T* p) { return p != nullptr; }
template<class T> struct TSubclassOf {
    const UClass* C;
    TSubclassOf(const UClass* c = nullptr): C(c) {}
    operator const UClass*() const { return C; }
    explicit operator bool() const { return C != nullptr; }
};
template<class T> struct TGuardValue { T& V; T Old; TGuardValue(T& v, T value): V(v), Old(v) { v = value; } ~TGuardValue() { V = Old; } };
struct FMath { static float Max(float a, float b) { return std::max(a, b); } };
struct UStateBase: Object { bool ExitAllowed = true; float Timer = 0; int Enters = 0, Exits = 0;
    std::function<void()> ExitHook;
    void Enter() { ++Enters; Timer = 0; }
    void Exit() { ++Exits; if (ExitHook) ExitHook(); }
    void Reset() { Timer = 0; }
};
struct UModularEnemyState: UStateBase { static const UClass* StaticClass() { static UClass c; return &c; } };
#define STATE(name) struct name: UModularEnemyState { static const UClass* StaticClass() { static UClass c{UModularEnemyState::StaticClass()}; return &c; } name() { Class = StaticClass(); } };
STATE(UModularEnemyExploreState);
STATE(UModularEnemyCombatState);
STATE(UModularEnemyDamageState);
STATE(UModularEnemyDeadState);
struct AModularEnemyAIController { bool Target = false; bool HasCombatTarget() const { return Target; } };
struct AModularEnemyCharacter: Object { bool Alive = true; AModularEnemyAIController AI;
    bool IsAlive() const { return Alive; }
    AModularEnemyAIController* GetEnemyController() const { return const_cast<AModularEnemyAIController*>(&AI); }
};
struct UStateMachine {
    Object* Owner = nullptr; std::vector<UStateBase*> States; UStateBase* Current = nullptr;
    Object* GetOwner() const { return Owner; }
    const std::vector<UStateBase*>& GetRegisteredStates() const { return States; }
    UStateBase* GetCurrentState() const { return Current; }
    template<class T> bool IsCurrentState() const { return Current && Current->IsA<T>(); }
    bool ForceChangeState(UStateBase* s) { if (Current == s) return false; if (Current) Current->Exit(); Current = s; s->Enter(); return true; }
    bool ChangeState(UStateBase* s) { return (!Current || Current->ExitAllowed) && ForceChangeState(s); }
    void Update(float dt) { if (Current) Current->Timer += dt; }
    void Stop() { if (Current) Current->Exit(); Current = nullptr; }
};
struct UModularEnemyStateMachine: UStateMachine {
    using Super = UStateMachine;
    bool bConfigured = true, bChangingState = false;
    bool ChangeEnemyState(TSubclassOf<UModularEnemyState>);
    bool ReturnToAction(); void Update(float); void NotifyDamage(); void Stop();
};
// PRODUCTION_METHODS
int main() {
    AModularEnemyCharacter enemy;
    UModularEnemyExploreState explore; UModularEnemyCombatState combat;
    UModularEnemyDamageState damage; UModularEnemyDeadState dead;
    UModularEnemyStateMachine fsm;
    fsm.Owner = &enemy; fsm.States = {&explore, &combat, &damage, &dead};
    assert(fsm.ReturnToAction() && fsm.Current == &explore);
    fsm.Update(.1f); assert(explore.Enters == 1); //同じステートは毎Tick入り直さない
    bool nestedAccepted = true;
    explore.ExitHook = [&] { nestedAccepted = fsm.ChangeEnemyState(UModularEnemyDeadState::StaticClass()); };
    enemy.AI.Target = true; fsm.Update(.1f);
    assert(fsm.Current == &combat && !nestedAccepted); //終了中の再帰要求を拒否
    combat.ExitAllowed = false;
    enemy.AI.Target = false; fsm.Update(.1f); assert(fsm.Current == &combat);
    fsm.NotifyDamage(); assert(fsm.Current == &damage); //被ダメージは終了ロックに割り込む
    fsm.Update(.3f); assert(fsm.Current == &damage);
    fsm.NotifyDamage(); assert(damage.Timer == 0 && damage.Enters == 1); //連続被弾はひるみ時間をリセット
    enemy.AI.Target = true; assert(fsm.ReturnToAction() && fsm.Current == &combat);
    enemy.Alive = false; fsm.NotifyDamage(); assert(fsm.Current == &dead);
    assert(!fsm.ChangeEnemyState(UModularEnemyExploreState::StaticClass())); //死亡後に復帰しない
    fsm.Update(1); assert(fsm.Current == &dead);
    fsm.Stop(); assert(!fsm.bConfigured && !fsm.Current);
    assert(!fsm.ReturnToAction());
    UModularEnemyStateMachine empty; empty.Owner = &enemy;
    assert(!empty.ChangeEnemyState(UModularEnemyCombatState::StaticClass())); //未登録へ遷移しない
    assert(!empty.ChangeEnemyState(nullptr));
}
