#include <algorithm>
#include <cassert>
struct UAnimMontage { bool Alive = true; };
struct UAnimInstance { bool Alive = true; };
template<class T> struct Ptr { T* P = nullptr; T* Get() const { return P; } bool IsValid() const { return P && P->Alive; } };
template<class T> bool IsValid(Ptr<T> p) { return p.IsValid(); }
struct FMath { static float Max(float a,float b){return std::max(a,b);} };
struct UPlayerStateMachine { int Returns=0;void ReturnToLocomotion(){++Returns;} };
struct UPlayerNormalAttackState {
    Ptr<UAnimMontage> AttackMontage;Ptr<UAnimInstance> PlayingAnimInstance;
    bool bAttackActive=true,bAttackCancelled=false,bMontageEnded=false;
    float DamageStartTime=.15f,DamageWindowDuration=.15f,ActiveDuration=.6f,m_stateTimer=0,PreviousAttackTime=0,CooldownRemaining=.3f;
    int Sweeps=0;UPlayerStateMachine Machine;
    void ApplyAttackHits(){++Sweeps;}UPlayerStateMachine* GetPlayerMachine(){return &Machine;}
    void OnUpdate_Implementation(float);void BackgroundUpdate(float);
    void OnAttackMontageEnded(UAnimMontage*,bool);void OnAttackMontageBlendingOut(UAnimMontage*,bool);
};
// PRODUCTION_METHODS
int main(){
    UPlayerNormalAttackState state;state.m_stateTimer=.1f;state.OnUpdate_Implementation(.1f);assert(state.Sweeps==0);
    state.m_stateTimer=.2f;state.OnUpdate_Implementation(.1f);assert(state.Sweeps==1);
    state.m_stateTimer=.4f;state.OnUpdate_Implementation(.2f);assert(state.Sweeps==2);
    state.m_stateTimer=.5f;state.OnUpdate_Implementation(.1f);assert(state.Sweeps==2);
    state.m_stateTimer=.7f;state.OnUpdate_Implementation(.2f);assert(!state.bAttackActive&&state.Machine.Returns==1);
    UPlayerNormalAttackState hitch;hitch.m_stateTimer=.7f;hitch.OnUpdate_Implementation(.7f);assert(hitch.Sweeps==1); //判定時間を飛び越えても1回調べる
    UPlayerNormalAttackState shortMontage;shortMontage.ActiveDuration=.1f;shortMontage.m_stateTimer=.2f;shortMontage.OnUpdate_Implementation(.2f);assert(shortMontage.Sweeps==0);
    UAnimMontage montage,other;UAnimInstance anim;UPlayerNormalAttackState cancelled;
    cancelled.AttackMontage.P=&montage;cancelled.PlayingAnimInstance.P=&anim;
    cancelled.OnAttackMontageBlendingOut(&other,true);assert(!cancelled.bAttackCancelled);
    cancelled.OnAttackMontageBlendingOut(&montage,true);cancelled.m_stateTimer=.2f;cancelled.OnUpdate_Implementation(.2f);
    assert(cancelled.Sweeps==0&&!cancelled.bAttackActive); //中断のブレンド中も当てない
    UPlayerNormalAttackState destroyed;destroyed.AttackMontage.P=&montage;destroyed.m_stateTimer=.2f;destroyed.OnUpdate_Implementation(.2f);assert(destroyed.Sweeps==0);
    UPlayerNormalAttackState ended;ended.OnAttackMontageEnded(nullptr,false);ended.m_stateTimer=.01f;ended.OnUpdate_Implementation(.01f);assert(!ended.bAttackActive);
    state.BackgroundUpdate(-1);assert(state.CooldownRemaining==.3f);state.BackgroundUpdate(1);assert(state.CooldownRemaining==0);
}
