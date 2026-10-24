#include <cassert>
using int32=int;
#define TEXT(x) x
struct FVector {float X=0;bool IsNearlyZero()const{return X==0;}};
struct AActor {virtual ~AActor()=default;bool Alive=true;FVector Location;FVector GetActorLocation()const{return Location;}};
template<class T>bool IsValid(T* p){return p&&p->Alive;}
template<class T>T* Cast(AActor* p){return dynamic_cast<T*>(p);}
struct Effect:AActor{int Hidden=0;void InvisibleEffect(){++Hidden;}};
struct AEnemy_Effect_1:Effect{};struct AEnemy_Effect_2:Effect{};struct AEnemy_Effect_3:Effect{};struct AEnemy_Effect_4:Effect{};
enum class EEnemyStatus{Patrol,Battle,Dead};
struct AEnemyBase:AActor{
    struct Info{int hp=100;bool bIsDead=false;}m_enemyInfo;
    bool m_deadCheck=true;EEnemyStatus m_enemyCurrentState=EEnemyStatus::Patrol;
    AActor* m_pPlayerChara=nullptr;FVector m_playerPos_LastSeen;AActor* CurrentStateEffect=nullptr;int Launches=0;
    bool IsDead()const{return m_enemyCurrentState==EEnemyStatus::Dead;}
    void LaunchCharacter(FVector,bool,bool){++Launches;}
    void OnDamage(int32,FVector,bool);
};
// PRODUCTION_METHODS
int main(){
    AActor player;player.Location.X=300;AEnemyBase enemy;enemy.m_pPlayerChara=&player;
    AEnemy_Effect_2 effect;enemy.CurrentStateEffect=&effect;
    enemy.OnDamage(25,{500},false);
    assert(enemy.m_enemyInfo.hp==75&&enemy.m_enemyCurrentState==EEnemyStatus::Battle&&enemy.Launches==1);
    assert(enemy.m_playerPos_LastSeen.X==300&&effect.Hidden==0); //非致死の通常攻撃で死亡演出を出さない
    enemy.OnDamage(0,{500},false);assert(enemy.m_enemyInfo.hp==75&&enemy.Launches==1);
    enemy.OnDamage(1000,{500},false);assert(enemy.IsDead()&&enemy.m_enemyInfo.hp==0&&effect.Hidden==1&&!enemy.CurrentStateEffect);
    enemy.OnDamage(10,{500},false);assert(effect.Hidden==1); //死亡後は再処理しない
    AEnemyBase sneak;sneak.OnDamage(10,{},true);assert(sneak.IsDead()&&sneak.m_enemyInfo.hp==0); //従来の即死は維持
    AEnemyBase restored;restored.m_enemyInfo.bIsDead=true;restored.OnDamage(20,{},false);assert(restored.m_enemyInfo.hp==100);
}
