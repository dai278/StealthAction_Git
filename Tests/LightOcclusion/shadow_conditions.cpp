#include <cassert>

struct FVector {};
struct APlayerCharacter
{
	bool m_bOnShadow = false;
	FVector GetFeetLocation() const { return {}; }
};
struct UExtendedSpotLightManager
{
	bool allHit = false, enemyHit = false;
	bool IsHitAllLight(FVector) const { return allHit; }
	bool IsHitEnemyLight(FVector) const { return enemyHit; }
};
struct UWorld
{
	UExtendedSpotLightManager* manager = nullptr;
	template <class T> T* GetSubsystem() { return manager; }
};
template <class T> bool IsValid(T* value) { return value != nullptr; }
struct UPlayerShadowState
{
	APlayerCharacter* player = nullptr;
	UWorld* world = nullptr;
	APlayerCharacter* GetPlayer() const { return player; }
	UWorld* GetWorld() const { return world; }
	bool IsFeetInShadow() const;
};

// PRODUCTION_METHOD

int main()
{
	APlayerCharacter player;
	UExtendedSpotLightManager lights;
	UWorld world{&lights};
	UPlayerShadowState state{&player, &world};
	assert(state.IsFeetInShadow());
	lights.allHit = true;
	assert(!state.IsFeetInShadow());
	player.m_bOnShadow = true;
	assert(state.IsFeetInShadow()); // 入れた直後、同じ条件で解除されない。
	lights.enemyHit = true;
	assert(!state.IsFeetInShadow());
	world.manager = nullptr;
	assert(!state.IsFeetInShadow());
	state.world = nullptr;
	assert(!state.IsFeetInShadow());
}
