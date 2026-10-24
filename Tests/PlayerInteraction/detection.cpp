#include <algorithm>
#include <cassert>
#include <vector>
struct FVector { double X = 0; static double DistSquared(FVector a, FVector b) { return (a.X-b.X)*(a.X-b.X); } };
struct FQuat {};
struct AActor { virtual ~AActor() = default; bool Alive = true; FVector Position; FVector GetActorLocation() const { return Position; } };
struct AInteract: AActor { bool Available = true; bool CanInteract() const { return Available; } };
template<class T> T* Cast(AActor* p) { return dynamic_cast<T*>(p); }
template<class T> bool IsValid(T* p) { return p && p->Alive; }
template<class T> struct TArray: std::vector<T> {
    using std::vector<T>::operator=;
    void Reset() { this->clear(); }
    void AddUnique(T p) { if (std::find(this->begin(), this->end(), p) == this->end()) this->push_back(p); }
};
constexpr int ECC_GameTraceChannel6 = 6;
struct FCollisionObjectQueryParams { void AddObjectTypesToQuery(int c) { assert(c == 6); } };
struct FCollisionQueryParams { void AddIgnoredActor(const AActor*) {} };
struct Shape {};
struct UCapsuleComponent { bool Alive = true, Query = true; bool IsQueryCollisionEnabled() const { return Query; }
    FVector GetComponentLocation() const { return {}; } FQuat GetComponentQuat() const { return {}; } Shape GetCollisionShape() const { return {}; } };
struct FOverlapResult { AActor* Actor; AActor* GetActor() const { return Actor; } };
struct UWorld { TArray<FOverlapResult> Overlaps;
    void OverlapMultiByObjectType(TArray<FOverlapResult>& out, FVector, FQuat, FCollisionObjectQueryParams, Shape, FCollisionQueryParams) { out = Overlaps; } };
struct APlayerCharacter: AActor {
    UCapsuleComponent Capsule; UWorld World;
    AInteract* m_hitInteractOb = nullptr; bool m_bHitIntteractObject = false;
    const UCapsuleComponent* GetCapsuleComponent() const { return &Capsule; }
    UWorld* GetWorld() const { return const_cast<UWorld*>(&World); }
    void GetNearbyInteractTargets(TArray<AInteract*>&) const;
    AInteract* FindInteractTarget() const;
    bool IsInInteractRange(const AInteract*) const;
    void RefreshInteractTarget();
};
// PRODUCTION_METHODS
int main() {
    APlayerCharacter player; AInteract a, b; AActor other;
    a.Position.X = 10; b.Position.X = 20;
    player.World.Overlaps = {{&a}, {&a}, {&b}, {&other}};
    TArray<AInteract*> targets; player.GetNearbyInteractTargets(targets);
    assert(targets.size() == 2); //複数コンポーネントでも対象を重複させない
    player.RefreshInteractTarget(); assert(player.m_hitInteractOb == &a);
    player.World.Overlaps = {{&b}}; player.RefreshInteractTarget();
    assert(player.m_hitInteractOb == &b && player.m_bHitIntteractObject); //片方の接触終了で他方を消さない
    player.World.Overlaps.clear(); player.RefreshInteractTarget();
    assert(!player.m_hitInteractOb && !player.m_bHitIntteractObject);
    player.World.Overlaps = {{&a}}; //影解除でサイズが戻って接触した状況。Beginイベント不要
    assert(player.FindInteractTarget() == &a); player.RefreshInteractTarget(); assert(player.m_hitInteractOb == &a);
    a.Available = false; assert(!player.IsInInteractRange(&a) && !player.FindInteractTarget());
    a.Available = true; a.Alive = false; assert(!player.FindInteractTarget());
    a.Alive = true; player.Capsule.Query = false; assert(!player.FindInteractTarget());
    player.Capsule.Query = true; player.World.Overlaps.clear();
    assert(!player.IsInInteractRange(&a)); //古い参照が残っていても範囲外なら拒否
}
