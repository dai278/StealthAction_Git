#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>
#define UE_LOG(...)
#define TEXT(x) x
struct FVector {
    double X = 0, Y = 0, Z = 0;
    static const FVector ZeroVector;
    FVector operator-(FVector b) const { return {X-b.X,Y-b.Y,Z-b.Z}; }
    bool IsNearlyZero() const { return Size() < .001; }
    double Size() const { return std::sqrt(X*X+Y*Y+Z*Z); }
    FVector GetSafeNormal() const { double n=Size();return n>0?FVector{X/n,Y/n,Z/n}:FVector{}; }
    FVector Rotation() const { return {}; }
};
const FVector FVector::ZeroVector{};
struct FMath { static float Max(float a,float b){return std::max(a,b);} static float Clamp(float v,float a,float b){return std::clamp(v,a,b);} static FVector RInterpTo(FVector,FVector,float,float){return {};} };
struct Object { bool Alive = true; virtual ~Object() = default; };
template<class T>bool IsValid(T* p){return p && p->Alive;}
template<class T>struct Weak {
    T* P = nullptr; Weak(T* p = nullptr):P(p){} bool IsValid() const {return ::IsValid(P);}
    T* Get() const {return IsValid()?P:nullptr;} T* operator->() const {return Get();}void Reset(){P=nullptr;}
};
struct APlayerCharacter;
struct AInteract: Object {bool Available=true, InRange=true; int Calls=0;FVector Position;
    bool CanInteract()const{return Available;} FVector GetInteractPosition(APlayerCharacter*){return Position;}
    void Interact(APlayerCharacter*){++Calls;Available=false;}
};
struct APlayerCharacter: Object {
    FVector Location{200,0,0};AInteract* Candidate=nullptr;int Moves=0;
    AInteract* FindInteractTarget()const{return IsInInteractRange(Candidate)?Candidate:nullptr;}
    bool IsInInteractRange(const AInteract* t)const{return IsValid(t)&&t->CanInteract()&&t->InRange;}
    FVector GetActorLocation()const{return Location;}FVector GetActorRotation()const{return {};}
    void AddMovementInput(FVector,float){++Moves;}void SetActorRotation(FVector){}
};
struct UPlayerInteractAnimationState;
struct UPlayerStateMachine: Object {
    int Active=0, Returns=0;bool AllowExit=true;UPlayerInteractAnimationState* Animation=nullptr;
    template<class T>bool IsCurrentState()const{return Active==1;}
    template<class T>T* GetState(){return static_cast<T*>(Animation);}
    void ReturnToLocomotion(){++Returns;if(AllowExit)Active=0;}void CancelDash(){}
};
struct StateBase {bool BlueprintAllows=true;bool CanEnter()const{return BlueprintAllows;}};
struct UAnimInstance: Object {};
struct UPlayerInteractAnimationState: StateBase {
    using Super=StateBase;APlayerCharacter* Player=nullptr;UPlayerStateMachine* Machine=nullptr;Weak<AInteract> InteractionTarget;Weak<UAnimInstance> PlayingAnimInstance;
    bool bInteractionExecuted=false,bMontageCancelled=false,bMontageStarted=false,bMontageEnded=false,bWaitForAnimationNotify=false;
    float m_stateTimer=0,InteractionDuration=2.5f;
    APlayerCharacter* GetPlayer()const{return Player;}UPlayerStateMachine* GetPlayerMachine()const{return Machine;}
    static int StaticClass(){return 1;}
    bool RequestPlayerState(int){if(!CanEnter())return false;Machine->Active=1;return true;}
    bool CanEnter()const;bool IsTargetAvailable()const;bool BeginInteraction(AInteract*);
    void OnUpdate_Implementation(float);void CompleteInteraction();
};
struct UPlayerInteractState: StateBase {
    using Super=StateBase;APlayerCharacter* Player=nullptr;UPlayerStateMachine* Machine=nullptr;Weak<AInteract> InteractionTarget;
    FVector InteractionPosition;float AcceptanceRadius=50,ApproachInputScale=.5f,RotationInterpSpeed=10;
    APlayerCharacter* GetPlayer()const{return Player;}UPlayerStateMachine* GetPlayerMachine()const{return Machine;}
    bool CanEnter()const;void OnEnter_Implementation();void OnUpdate_Implementation(float);
};
// PRODUCTION_METHODS
int main(){
    APlayerCharacter player;UPlayerStateMachine machine;AInteract target,other;
    UPlayerInteractAnimationState animation;animation.Player=&player;animation.Machine=&machine;machine.Animation=&animation;
    UPlayerInteractState approach;approach.Player=&player;approach.Machine=&machine;
    assert(!approach.CanEnter()&&!animation.CanEnter()&&!animation.BeginInteraction(nullptr)); //対象なしでは開始しない
    player.Candidate=&target;assert(approach.CanEnter());
    target.Position={400,0,0};approach.OnEnter_Implementation();approach.OnUpdate_Implementation(.1f);
    assert(player.Moves==1&&machine.Active==0); //対象に到着する前はモンタージュへ進まない
    target.InRange=false;approach.OnUpdate_Implementation(.1f);assert(machine.Returns==1&&!animation.CanEnter());
    target.InRange=true;target.Position=FVector::ZeroVector;approach.OnEnter_Implementation();
    approach.OnUpdate_Implementation(.1f);assert(machine.Active==1); //ゼロ指定は有効な接触対象がある場合だけ許可
    assert(!animation.BeginInteraction(&other)&&animation.InteractionTarget.Get()==&target); //開始中の対象を上書きしない
    target.InRange=false;animation.m_stateTimer=10;animation.OnUpdate_Implementation(.1f);
    assert(machine.Active==0&&target.Calls==0); //範囲外へ離れたら実行しない
    animation.CompleteInteraction();assert(target.Calls==0); //中断後の遅れた通知も無効
    target.InRange=true;assert(animation.BeginInteraction(&target));animation.m_stateTimer=10;
    machine.AllowExit=false;animation.OnUpdate_Implementation(.1f);assert(target.Calls==1&&!target.Available);
    animation.OnUpdate_Implementation(.1f);assert(target.Calls==1); //対象が使用不可になっても二重実行しない
    machine.AllowExit=true;animation.OnUpdate_Implementation(.1f);assert(machine.Active==0);
    target.Available=true;animation.bInteractionExecuted=false;assert(animation.BeginInteraction(&target));
    target.Available=false;animation.CompleteInteraction();assert(target.Calls==1&&machine.Active==0); //開始後に使用不可になったらキャンセル
    target.Available=true;animation.BlueprintAllows=false;assert(!animation.BeginInteraction(&target));assert(!animation.InteractionTarget.IsValid());
    animation.BlueprintAllows=true;target.InRange=false;animation.InteractionTarget=&target;
    assert(!animation.CanEnter()); //BPの条件がtrueでも範囲外は不可
}
