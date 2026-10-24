#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <string>
#define TEXT(x) x
#define UE_LOG(...)
struct FName {std::string Value;FName(const char* s=""):Value(s){}bool IsNone()const{return Value.empty();}};
struct FLinearColor{float R=0,G=0,B=0,A=1;FLinearColor()=default;FLinearColor(float r,float g,float b,float a):R(r),G(g),B(b),A(a){}static const FLinearColor Black;};
const FLinearColor FLinearColor::Black{};
struct FSlateColor{FLinearColor Color;FSlateColor(FLinearColor c):Color(c){}};
struct SBorder{FLinearColor Color;void SetBorderBackgroundColor(FSlateColor c){Color=c.Color;}};
namespace EVisibility {constexpr int HitTestInvisible=0;}
struct Builder{FLinearColor Color;
    Builder& BorderImage(int){return *this;}Builder& BorderBackgroundColor(FSlateColor c){Color=c.Color;return *this;}Builder& Visibility(int){return *this;}};
#define SNew(Type) Builder()
struct FCoreStyle{static FCoreStyle Get(){return {};}int GetBrush(const char*){return 0;}};
template<class T>struct Shared{std::shared_ptr<T> P;bool IsValid()const{return bool(P);}T* operator->()const{return P.get();}
    Shared& operator=(const Builder& b){P=std::make_shared<T>();P->Color=b.Color;return *this;}
    std::shared_ptr<T> ToSharedRef()const{return P;}void Reset(){P.reset();}};
template<class T>struct Weak{T* P=nullptr;T* Get()const{return P;}Weak& operator=(T* p){P=p;return *this;}void Reset(){P=nullptr;}};
template<class T>bool IsValid(T* p){return p!=nullptr;}
struct FMath{static float Max(float a,float b){return std::max(a,b);}static float Clamp(float v,float a,float b){return std::clamp(v,a,b);}};
struct UGameViewportClient{int Adds=0,Removes=0;std::shared_ptr<SBorder> Widget;
    void AddViewportWidgetContent(std::shared_ptr<SBorder> p,int z){assert(z==100000);Widget=p;++Adds;}
    void RemoveViewportWidgetContent(std::shared_ptr<SBorder> p){if(Widget==p)Widget.reset();++Removes;}};
struct UGameInstance;
struct UWorld{UGameInstance* Instance=nullptr;UGameInstance* GetGameInstance(){return Instance;}};
struct UGameInstance{UGameViewportClient* Viewport=nullptr;UWorld* World=nullptr;int Opens=0;std::string Destination;std::function<void()> Loaded;
    UGameViewportClient* GetGameViewportClient(){return Viewport;}UWorld* GetWorld()const{return World;}};
struct UGameplayStatics{static void OpenLevel(UGameInstance* instance,FName name){++instance->Opens;instance->Destination=name.Value;if(instance->Loaded)instance->Loaded();}};
struct FApp{static float Delta;static float GetDeltaTime(){return Delta;}};float FApp::Delta=0;
namespace ETravelFailure{enum Type{InvalidURL};}
using FString=std::string;
struct ULevelTransitionSubsystem{
    enum class EFadeStage{None,FadeOut,Holding,Loading,FadeIn};EFadeStage Stage=EFadeStage::None;
    UGameInstance* Instance=nullptr;UGameInstance* GetGameInstance()const{return Instance;}
    Shared<SBorder> FadeWidget;Weak<UGameViewportClient> FadeViewport;FName DestinationLevel;FLinearColor FadeColor;
    float Elapsed=0,FadeOutDuration=0,HoldDuration=0,FadeInDuration=0;
    bool OpenLevelWithFade(FName,float,float,float,FLinearColor);bool IsTransitionActive()const;UWorld* GetTickableGameObjectWorld()const;
    void Tick(float);void AdvanceFade(float);void SetFadeOpacity(float);void FinishTransition();void OnMapLoaded(UWorld*);void OnTravelFailed(UWorld*,ETravelFailure::Type,const FString&);
};
// PRODUCTION_METHODS
int main(){
    UGameViewportClient viewport;UGameInstance game;UWorld world;game.Viewport=&viewport;game.World=&world;world.Instance=&game;
    ULevelTransitionSubsystem fade;fade.Instance=&game;
    assert(fade.OpenLevelWithFade("GameOver",.5f,.1f,.5f,FLinearColor::Black));
    assert(fade.OpenLevelWithFade("GameClear",1,1,1,FLinearColor::Black));assert(fade.DestinationLevel.Value=="GameOver"&&viewport.Adds==1);
    FApp::Delta=.25f;fade.Tick(0);assert(viewport.Widget->Color.A==.5f&&game.Opens==0); //ポーズ中も実時間で進む
    fade.Tick(0);assert(viewport.Widget->Color.A==1&&game.Opens==0);
    fade.AdvanceFade(.05f);assert(game.Opens==0);fade.AdvanceFade(.05f);assert(game.Opens==1&&game.Destination=="GameOver");
    fade.AdvanceFade(5);assert(game.Opens==1&&viewport.Widget->Color.A==1); //ロード待ちでは再移動しない
    UGameInstance other;UWorld otherWorld;otherWorld.Instance=&other;fade.OnMapLoaded(&otherWorld);assert(fade.Stage==ULevelTransitionSubsystem::EFadeStage::Loading);
    fade.OnMapLoaded(&world);assert(viewport.Adds==2&&viewport.Widget->Color.A==1);
    fade.AdvanceFade(.25f);assert(viewport.Widget->Color.A==.5f);fade.AdvanceFade(.25f);assert(!fade.IsTransitionActive()&&!viewport.Widget);
    game.Loaded=[&]{fade.OnMapLoaded(&world);};
    assert(fade.OpenLevelWithFade("GameClear",0,0,0,FLinearColor::Black));fade.AdvanceFade(0);fade.AdvanceFade(0);
    assert(game.Opens==2&&!fade.IsTransitionActive()); //同期ロードとゼロ秒でも暗転を残さない
    game.Loaded={};fade.OpenLevelWithFade("BadMap",.5f,0,.5f,FLinearColor::Black);fade.OnTravelFailed(&otherWorld,ETravelFailure::InvalidURL,"");assert(fade.IsTransitionActive());
    fade.OnTravelFailed(nullptr,ETravelFailure::InvalidURL,"");assert(!fade.IsTransitionActive()&&!viewport.Widget);
    fade.OpenLevelWithFade("GameClear",1,0,1,FLinearColor::Black);fade.OnMapLoaded(&world);assert(!fade.IsTransitionActive()); //暗転途中の別の移動も後片付けする
    game.Viewport=nullptr;assert(!fade.OpenLevelWithFade("GameClear",1,0,1,FLinearColor::Black));
}
