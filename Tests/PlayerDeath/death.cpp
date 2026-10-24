#include <algorithm>
#include <cassert>
#include <string>
#define TEXT(x) x
#define UE_LOG(...)
struct FName{std::string Name;FName(const char* n="GameOver"):Name(n){}bool IsNone()const{return Name.empty();}};
struct FLinearColor{};
namespace EMontagePlayReturnType {constexpr int Duration=1;}
struct FMath{static float Max(float a,float b){return std::max(a,b);}};
struct UAnimMontage{float Length=2;};
template<class T>struct Ptr{T* P=nullptr;T* Get()const{return P;}};
template<class T>bool IsValid(T* p){return p!=nullptr;}
template<class T>bool IsValid(Ptr<T> p){return p.P!=nullptr;}
template<class T>struct Weak{T* P=nullptr;Weak& operator=(T* p){P=p;return *this;}T* Get()const{return P;}void Reset(){P=nullptr;}};
struct UAnimInstance{bool CanPlay=true,Active=false;int Plays=0,Stops=0;float Montage_Play(UAnimMontage* m,float rate,int){++Plays;Active=CanPlay;return CanPlay?m->Length/rate:0;}bool Montage_IsActive(UAnimMontage*){return Active;}void Montage_Stop(float,UAnimMontage*){Active=false;++Stops;}};
struct USkeletalMeshComponent{UAnimInstance* Anim=nullptr;UAnimInstance* GetAnimInstance(){return Anim;}};
struct UCharacterMovementComponent{int Stops=0;void StopMovementImmediately(){++Stops;}};
struct UPlayerStateMachine{int Cancels=0;void CancelLocomotionRequests(){++Cancels;}};
struct ULevelTransitionSubsystem{bool Available=true;int Calls=0;bool OpenLevelWithFade(FName,float,float,float,FLinearColor){++Calls;return Available;}};
struct UGameInstance{ULevelTransitionSubsystem* Transition=nullptr;template<class T>T* GetSubsystem(){return Transition;}};
struct APlayerCharacter{
    struct Info{bool isAlive=true;int bIsGetKeyItem=1;}m_playerInfo;
    USkeletalMeshComponent Mesh;UCharacterMovementComponent Movement;UGameInstance Instance;int Opens=0;
    USkeletalMeshComponent* GetMesh(){return &Mesh;}UCharacterMovementComponent* GetCharacterMovement(){return &Movement;}UGameInstance* GetGameInstance(){return &Instance;}
};
struct UGameplayStatics{static void OpenLevel(APlayerCharacter* player,FName){++player->Opens;}};
struct UPlayerDeadState{
    APlayerCharacter* Player=nullptr;UPlayerStateMachine Machine;Ptr<UAnimMontage> DeathMontage;Weak<UAnimInstance> PlayingAnimInstance;
    float MontagePlayRate=1,GameOverDelay=0,FadeStartTime=0,m_stateTimer=0,FadeOutDuration=.5f,BlackHoldDuration=.1f,FadeInDuration=.5f;
    bool bWaitForDeathMontage=true,bTransitionRequested=false,bOpenGameOverLevel=true,HasWorld=true;FName GameOverLevel;FLinearColor FadeColor;
    APlayerCharacter* GetPlayer(){return Player;}UPlayerStateMachine* GetPlayerMachine(){return &Machine;}void* GetWorld(){return HasWorld?this:nullptr;}
    void OnEnter_Implementation();void OnUpdate_Implementation(float);void BeginGameOverTransition();void OnExit_Implementation();
};
// PRODUCTION_METHODS
int main(){
    APlayerCharacter player;UAnimInstance anim;UAnimMontage montage;ULevelTransitionSubsystem transition;
    player.Mesh.Anim=&anim;player.Instance.Transition=&transition;
    UPlayerDeadState state;state.Player=&player;state.DeathMontage.P=&montage;state.MontagePlayRate=2;state.GameOverDelay=.25f;
    state.OnEnter_Implementation();assert(anim.Plays==1&&state.FadeStartTime==1.25f&&transition.Calls==0&&!player.m_playerInfo.isAlive&&!player.m_playerInfo.bIsGetKeyItem);
    state.m_stateTimer=1;state.OnUpdate_Implementation(1);assert(transition.Calls==0);
    state.m_stateTimer=1.25f;state.OnUpdate_Implementation(.25f);state.OnUpdate_Implementation(.25f);assert(transition.Calls==1&&player.Opens==0);
    state.OnExit_Implementation();assert(anim.Stops==1&&!state.PlayingAnimInstance.Get());
    UPlayerDeadState noWait;noWait.Player=&player;noWait.DeathMontage.P=&montage;noWait.bWaitForDeathMontage=false;noWait.OnEnter_Implementation();assert(transition.Calls==2);
    UPlayerDeadState failure;failure.Player=&player;failure.DeathMontage.P=&montage;anim.CanPlay=false;failure.OnEnter_Implementation();assert(transition.Calls==3&&failure.FadeStartTime==0);
    UPlayerDeadState missing;missing.Player=&player;missing.OnEnter_Implementation();assert(transition.Calls==4); //未設定でも従来の暗転へ進む
    transition.Available=false;UPlayerDeadState fallback;fallback.Player=&player;fallback.OnEnter_Implementation();assert(player.Opens==1);fallback.OnUpdate_Implementation(1);assert(player.Opens==1);
    UPlayerDeadState disabled;disabled.Player=&player;disabled.bOpenGameOverLevel=false;disabled.OnEnter_Implementation();disabled.m_stateTimer=10;disabled.OnUpdate_Implementation(10);assert(player.Opens==1);
    UPlayerDeadState nullOwner;nullOwner.OnEnter_Implementation();nullOwner.OnUpdate_Implementation(10);assert(!nullOwner.bTransitionRequested);
}
