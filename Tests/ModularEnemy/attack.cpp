#include <algorithm>
#include <cassert>
struct FVector {
    float X = 0, Y = 0, Z = 0;
    static const FVector ZeroVector;
    static float DistSquared(const FVector& a, const FVector& b) {
        return (a.X-b.X)*(a.X-b.X) + (a.Y-b.Y)*(a.Y-b.Y) + (a.Z-b.Z)*(a.Z-b.Z);
    }
};
const FVector FVector::ZeroVector{};
struct FMath {
    static float Max(float a, float b) { return std::max(a, b); }
    static float Square(float a) { return a*a; }
};
struct AActor { virtual ~AActor() = default; FVector Location; FVector GetActorLocation() const { return Location; } };
struct IDamageable { virtual void OnDamage(int, FVector, bool) = 0; virtual ~IDamageable() = default; };
template<class T> T* Cast(AActor* p) { return dynamic_cast<T*>(p); }
template<class T> struct Ptr {
    T* P = nullptr; Ptr(T* p = nullptr): P(p) {}
    T* Get() const { return P; } operator T*() const { return P; }
    void Reset() { P = nullptr; }
};
template<class T> bool IsValid(T* p) { return p != nullptr; }
template<class T> bool IsValid(Ptr<T> p) { return p.P != nullptr; }
struct UAnimMontage {};
struct UModularEnemyCombatState {};
struct UModularEnemyStateMachine { bool Combat = true; template<class T> bool IsCurrentState() const { return Combat; } };
struct AModularEnemyAIController {
    AActor* Target = nullptr; bool Visible = true, ClearSight = true; int MoveStops = 0;
    UModularEnemyStateMachine Machine;
    UModularEnemyStateMachine* GetEnemyStateMachine() { return &Machine; }
    AActor* GetVisibleTarget() { return Visible ? Target : nullptr; }
    bool LineOfSightTo(AActor*) { return ClearSight; }
    void StopMovement() { ++MoveStops; }
};
struct CharacterBase: AActor { void Tick(float) {} };
struct AModularEnemyCharacter: CharacterBase {
    using Super = CharacterBase;
    bool Alive = true, bAttackActive = false, bDamageApplied = false;
    float CooldownRemaining = 0, AttackElapsed = 0, AttackWindup = .3f, AttackDuration = .8f, AttackRange = 150, AttackCooldown = .5f;
    int AttackDamage = 10, MontageStops = 0;
    Ptr<AActor> AttackTarget; Ptr<UAnimMontage> AttackMontage; AModularEnemyAIController* AI = nullptr;
    bool IsAlive() const { return Alive; }
    AModularEnemyAIController* GetEnemyController() { return AI; }
    void PlayAnimMontage(UAnimMontage*) {}
    void StopAnimMontage(UAnimMontage*) { ++MontageStops; }
    void Tick(float); bool BeginAttack(); void CancelAttack();
};
struct Target: AActor, IDamageable { int Hits = 0, Damage = 0;
    void OnDamage(int amount, FVector, bool) override { ++Hits; Damage += amount; }
};
// PRODUCTION_METHODS
int main() {
    Target target; target.Location.X = 100;
    AModularEnemyAIController ai; ai.Target = &target;
    UAnimMontage montage;
    AModularEnemyCharacter enemy; enemy.AI = &ai; enemy.AttackMontage = &montage;
    assert(enemy.BeginAttack() && !enemy.BeginAttack());
    enemy.Tick(.2f); assert(target.Hits == 0);
    enemy.Tick(.2f); assert(target.Hits == 1 && target.Damage == 10);
    enemy.Tick(.2f); assert(target.Hits == 1);
    enemy.CancelAttack(); enemy.Tick(1); assert(target.Hits == 1 && enemy.MontageStops == 1);
    assert(enemy.BeginAttack()); enemy.CancelAttack(); enemy.Tick(1); assert(target.Hits == 1); //構え中の中断では当てない
    assert(enemy.BeginAttack()); ai.ClearSight = false; enemy.Tick(1); assert(target.Hits == 1); //壁越しには当てない
    enemy.Tick(1); ai.ClearSight = true; assert(enemy.BeginAttack());
    ai.Visible = false; enemy.Tick(1); assert(target.Hits == 1); //見失った相手へ当てない
    enemy.Tick(1); ai.Visible = true; ai.Machine.Combat = false; assert(!enemy.BeginAttack());
    ai.Machine.Combat = true; target.Location.X = 1000; assert(!enemy.BeginAttack());
    target.Location.X = 100; assert(enemy.BeginAttack()); enemy.Alive = false; enemy.Tick(.5f);
    assert(target.Hits == 1 && !enemy.bAttackActive); //死亡後は予約済み攻撃も残さない
    AModularEnemyCharacter other; other.AI = &ai; assert(other.BeginAttack()); other.Tick(.4f);
    assert(target.Hits == 2); //別の敵の攻撃は独立
}
