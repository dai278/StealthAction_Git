#include <cassert>
#include <type_traits>
#define UE_LOG(...)
#define TEXT(x) x
struct UPlayerShadowState {};
struct UPlayerAttackState { bool Allowed = true; bool CanEnter() const { return Allowed; } static int StaticClass() { return 1; } };
struct UPlayerNormalAttackState { static int StaticClass() { return 2; } };
struct UPlayerStateMachine {
    UPlayerAttackState Sneak; UPlayerNormalAttackState Normal;
    bool Control = true, Shadow = false, CanLeaveShadow = true, HasNormal = true, AcceptChange = true;
    int LastChange = 0, Changes = 0;
    bool CanControl() const { return Control; }
    template<class T> bool IsCurrentState() const { return Shadow; }
    template<class T> T* GetState() {
        if constexpr (std::is_same_v<T, UPlayerAttackState>) return &Sneak;
        else return HasNormal ? &Normal : nullptr;
    }
    bool ReturnToLocomotion() { if (!CanLeaveShadow) return false; Shadow = false; return true; }
    bool ChangePlayerState(int state) { ++Changes; LastChange = state; return AcceptChange; }
    void TryAttack();
};
// PRODUCTION_METHODS
int main() {
    UPlayerStateMachine machine;
    machine.TryAttack(); assert(machine.LastChange == 1); //ステルスキル可能なら従来の攻撃
    machine.Sneak.Allowed = false; machine.TryAttack(); assert(machine.LastChange == 2); //発見中等は通常攻撃
    int count = machine.Changes;
    machine.Control = false; machine.TryAttack(); assert(machine.Changes == count);
    machine.Control = true; machine.Shadow = true; machine.CanLeaveShadow = false;
    machine.TryAttack(); assert(machine.Changes == count);
    machine.CanLeaveShadow = true; machine.TryAttack(); assert(!machine.Shadow && machine.LastChange == 2);
    machine.HasNormal = false; count = machine.Changes; machine.TryAttack(); assert(machine.Changes == count);
    machine.HasNormal = true; machine.Sneak.Allowed = true; machine.AcceptChange = false;
    count = machine.Changes; machine.TryAttack(); assert(machine.Changes == count + 1 && machine.LastChange == 1); //遷移拒否を別攻撃で回避しない
}
