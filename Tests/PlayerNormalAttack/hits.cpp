#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>
struct FVector {
    float X=0,Y=0,Z=0;
    FVector operator+(FVector v)const{return {X+v.X,Y+v.Y,Z+v.Z};}
    FVector operator-(FVector v)const{return {X-v.X,Y-v.Y,Z-v.Z};}
    FVector operator*(float s)const{return {X*s,Y*s,Z*s};}
    FVector GetSafeNormal()const{return {1,0,0};}
};
struct FTransform{FVector TransformVectorNoScale(FVector v)const{return v;}};
struct FQuat{static const FQuat Identity;};const FQuat FQuat::Identity{};
struct FMath{static float Max(float a,float b){return std::max(a,b);}};
struct AActor{virtual ~AActor()=default;bool Alive=true,BehindWall=false;FVector Location;FVector GetActorLocation()const{return Location;}};
struct IDamageable{virtual ~IDamageable()=default;virtual void OnDamage(int,FVector,bool)=0;};
template<class T>T* Cast(AActor* p){return dynamic_cast<T*>(p);}
template<class T>bool IsValid(T* p){return p&&p->Alive;}
template<class T>struct TWeakObjectPtr{T* P;TWeakObjectPtr(T* p):P(p){}bool operator==(TWeakObjectPtr other)const{return P==other.P;}};
template<class T>struct TSet{std::vector<T> Values;bool Contains(T v)const{return std::find(Values.begin(),Values.end(),v)!=Values.end();}void Add(T v){Values.push_back(v);}void Reset(){Values.clear();}};
template<class T>using TArray=std::vector<T>;
constexpr int ECC_GameTraceChannel2=2,ECC_WorldStatic=0,ECC_WorldDynamic=1;
struct FCollisionObjectQueryParams{std::vector<int> Channels;void AddObjectTypesToQuery(int n){Channels.push_back(n);}};
struct FCollisionQueryParams{AActor* Last=nullptr;void AddIgnoredActor(AActor* p){Last=p;}};
struct FCollisionShape{static FCollisionShape MakeSphere(float radius){assert(radius>=1);return {};}};
struct FHitResult{AActor* Actor=nullptr;AActor* GetActor()const{return Actor;}};
struct UWorld{TArray<FHitResult> Candidates;
    void SweepMultiByObjectType(TArray<FHitResult>& hits,FVector,FVector,FQuat,FCollisionObjectQueryParams query,FCollisionShape,FCollisionQueryParams){assert(query.Channels==std::vector<int>{2});hits=Candidates;}
    bool LineTraceSingleByObjectType(FHitResult&,FVector,FVector,FCollisionObjectQueryParams query,FCollisionQueryParams params){assert(query.Channels==std::vector<int>({0,1}));return params.Last->BehindWall;}
};
struct APlayerCharacter: AActor{FTransform GetActorTransform()const{return {};}FVector GetActorForwardVector()const{return {1,0,0};}};
struct Victim: AActor,IDamageable{int Hits=0;bool Sneak=true;std::function<void()> Hook;void OnDamage(int damage,FVector,bool sneak)override{assert(damage==10);++Hits;Sneak=sneak;if(Hook)Hook();}};
struct AEnemyBase: Victim{bool Dead=false;bool IsDead()const{return Dead;}};
struct AModularEnemyCharacter: Victim{bool Living=true;bool IsAlive()const{return Living;}};
struct UPlayerNormalAttackState{
    APlayerCharacter Player;UWorld World;TSet<TWeakObjectPtr<AActor>> HitActors;
    FVector AttackOffset{40,0,0};float AttackDistance=100,AttackRadius=50,KnockbackForce=500;int AttackDamage=10;bool bAttackActive=true,bAttackCancelled=false;
    APlayerCharacter* GetPlayer(){return &Player;}UWorld* GetWorld(){return &World;}void ApplyAttackHits();
};
// PRODUCTION_METHODS
int main(){
    UPlayerNormalAttackState state;AEnemyBase old,dead;AModularEnemyCharacter modular,deadModular;AActor wall;
    dead.Dead=true;deadModular.Living=false;
    state.World.Candidates={{&old},{&old},{&modular},{&dead},{&deadModular},{&wall},{&state.Player}};
    state.ApplyAttackHits();state.ApplyAttackHits();
    assert(old.Hits==1&&modular.Hits==1&&!old.Sneak&&!modular.Sneak); //複数コリジョン・複数フレームでも1回ずつ
    assert(dead.Hits==0&&deadModular.Hits==0);
    state.HitActors.Reset();old.BehindWall=true;state.ApplyAttackHits();assert(old.Hits==1&&modular.Hits==2); //壁越しには当てない
    state.HitActors.Reset();old.BehindWall=false;old.Hook=[&]{state.bAttackCancelled=true;};state.ApplyAttackHits();
    assert(old.Hits==2&&modular.Hits==2); //ダメージ通知中に中断されたら残りの対象へ当てない
}
