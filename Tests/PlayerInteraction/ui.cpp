#include <cassert>
#include <cmath>
#include <functional>

using int32 = int;
using FName = const char*;
namespace EEndPlayReason { enum Type { Destroyed }; }
struct FVector
{
	double X = 0, Y = 0, Z = 0;
	static double Distance(FVector a, FVector b)
	{
		return std::sqrt((a.X-b.X)*(a.X-b.X) + (a.Y-b.Y)*(a.Y-b.Y) + (a.Z-b.Z)*(a.Z-b.Z));
	}
};
struct FHitResult {};
struct FTimerHandle {};
struct TimerManager
{
	std::function<void()> callback;
	template <class T> void SetTimer(FTimerHandle&, T* object, void (T::*method)(), float interval, bool loop)
	{
		assert(interval == .1f && loop);
		callback = [object, method] { std::invoke(method, object); };
	}
	void ClearTimer(FTimerHandle&) { callback = {}; }
	void Fire() { if (callback) callback(); }
};
struct UWorld
{
	TimerManager timer;
	TimerManager& GetTimerManager() { return timer; }
};
struct AActor
{
	virtual ~AActor() = default;
	virtual void BeginPlay() {}
	virtual void EndPlay(EEndPlayReason::Type) {}
	FVector position;
	UWorld* world = nullptr;
	FVector GetActorLocation() const { return position; }
	UWorld* GetWorld() const { return world; }
};
struct APawn : AActor {};
struct AInteract;
struct APlayerCharacter : APawn
{
	bool shadow = false, inRange = true;
	mutable int queries = 0;
	bool IsInShadow() const { return shadow; }
	bool IsInInteractRange(const AInteract*) const { ++queries; return inRange; }
};
template <class T, class U> T* Cast(U* value) { return dynamic_cast<T*>(value); }
template <class T> bool IsValid(T* value) { return value != nullptr; }
struct UGameplayStatics
{
	static APlayerCharacter* player;
	static APlayerCharacter* GetPlayerCharacter(const AActor*, int) { return player; }
};
APlayerCharacter* UGameplayStatics::player = nullptr;
struct Delegate
{
	template <class T, class F> void AddDynamic(T*, F) {}
};
struct UPrimitiveComponent {};
struct UBoxComponent : UPrimitiveComponent
{
	Delegate OnComponentBeginOverlap, OnComponentEndOverlap;
	void SetCollisionProfileName(FName) {}
};
struct UWidgetComponent
{
	bool visible = true;
	int changes = 0;
	void SetVisibility(bool value) { visible = value; ++changes; }
	bool IsVisible() const { return visible; }
};
struct AInteract : AActor
{
	using Super = AActor;
	UWidgetComponent* m_intractUI = nullptr;
	UBoxComponent* m_pCollision = nullptr;
	APawn* m_pPlayer = nullptr;
	FTimerHandle m_uiUpdateTimer;
	float m_visibleUILenght = 1000;
	bool usable = true;
	bool CanInteract() const { return usable; }
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type) override;
	void UpdateVisible();
	void OnOverlapBegin(UPrimitiveComponent*, AActor*, UPrimitiveComponent*, int32, bool, const FHitResult&);
	void OnOverlapEnd(UPrimitiveComponent*, AActor*, UPrimitiveComponent*, int32);
};

// PRODUCTION_METHODS

int main()
{
	UWorld world;
	UBoxComponent collision;
	UWidgetComponent widget;
	AInteract target;
	target.world = &world;
	target.m_intractUI = &widget;
	target.m_pCollision = &collision;
	target.BeginPlay();
	assert(!widget.visible && world.timer.callback);

	// ActorのTickやBeginOverlapがなくても、生成されたプレイヤーを検出する。
	APlayerCharacter player;
	UGameplayStatics::player = &player;
	world.timer.Fire();
	assert(widget.visible && target.m_pPlayer == &player);
	const int changes = widget.changes;
	world.timer.Fire();
	assert(widget.changes == changes);

	// 敵の出入りは無視し、プレイヤーのメッシュだけが抜けても隠さない。
	AActor enemy;
	const int queries = player.queries;
	target.OnOverlapEnd(nullptr, &enemy, nullptr, 0);
	assert(widget.visible && player.queries == queries);
	target.OnOverlapEnd(nullptr, &player, nullptr, 0);
	assert(widget.visible);

	// 距離制限で隠れた後、再入場イベントがなくても戻る。
	player.position.X = 1500;
	world.timer.Fire();
	assert(!widget.visible);
	player.position.X = 0;
	world.timer.Fire();
	assert(widget.visible);

	player.inRange = false;
	target.OnOverlapEnd(nullptr, &player, nullptr, 0);
	assert(!widget.visible);
	target.OnOverlapBegin(nullptr, &enemy, nullptr, 0, false, {});
	assert(!widget.visible);
	player.inRange = true;
	target.OnOverlapBegin(nullptr, &player, nullptr, 0, false, {});
	assert(widget.visible);

	player.shadow = true;
	world.timer.Fire();
	assert(!widget.visible);
	player.shadow = false;
	world.timer.Fire();
	assert(widget.visible);
	target.usable = false;
	world.timer.Fire();
	assert(!widget.visible);
	target.usable = true;

	APlayerCharacter respawned;
	UGameplayStatics::player = &respawned;
	world.timer.Fire();
	assert(widget.visible && target.m_pPlayer == &respawned);
	UGameplayStatics::player = nullptr;
	world.timer.Fire();
	assert(!widget.visible);
	target.EndPlay(EEndPlayReason::Destroyed);
	assert(!world.timer.callback);
}
