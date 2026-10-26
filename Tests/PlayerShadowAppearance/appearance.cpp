#include <cassert>
#include <cmath>
#include <functional>
#include <vector>

using int32 = int;
struct FVector
{
	double X = 0, Y = 0, Z = 0;
	FVector() = default;
	FVector(double x, double y, double z) : X(x), Y(y), Z(z) {}
	FVector operator+(FVector b) const { return {X+b.X, Y+b.Y, Z+b.Z}; }
	static double Distance(FVector a, FVector b)
	{
		return std::sqrt((a.X-b.X)*(a.X-b.X) + (a.Y-b.Y)*(a.Y-b.Y) + (a.Z-b.Z)*(a.Z-b.Z));
	}
};
template <class T> struct TObjectPtr
{
	T* pointer = nullptr;
	T* Get() const { return pointer; }
};
template <class T> struct TArray : std::vector<T>
{
	int32 Num() const { return static_cast<int32>(this->size()); }
	void Reset() { this->clear(); }
};
template <class T> bool IsValid(T* pointer) { return pointer != nullptr; }
struct UMaterialInterface {};
struct USkeletalMeshComponent
{
	bool visible = false, hidden = true, bPauseAnims = true, overlap = false;
	int collision = 0;
	FVector position;
	std::vector<UMaterialInterface*> materials;
	std::function<void()> onOverlap;
	FVector GetComponentLocation() const { return position; }
	void SetHiddenInGame(bool value, bool) { hidden = value; }
	void SetRelativeLocation(FVector value) { position = value; }
	void SetMaterial(int32 index, UMaterialInterface* value) { materials.at(index) = value; }
	void SetVisibility(bool value) { visible = value; }
	void SetCollisionEnabled(int value) { collision = value; }
	void SetGenerateOverlapEvents(bool value)
	{
		overlap = value;
		if (onOverlap) onOverlap();
	}
};
struct UCameraComponent
{
	FVector position;
	FVector GetComponentLocation() const { return position; }
};
struct UCapsuleComponent
{
	float radius = 10, halfHeight = 10;
	int profile = 0;
	void SetCapsuleSize(float r, float h) { radius = r; halfHeight = h; }
	float GetScaledCapsuleHalfHeight() const { return halfHeight; }
	void SetCollisionProfileName(int value) { profile = value; }
};
enum class ECameraStatus { ThirdPerson, InShadow };
struct Event
{
	int calls = 0;
	std::function<void()> callback;
	void Broadcast() { ++calls; if (callback) callback(); }
};
struct APlayerCharacter
{
	USkeletalMeshComponent mesh;
	UCameraComponent camera;
	UCapsuleComponent capsule;
	FVector feet;
	bool shadow = true, m_bCameraSwitching = false, refreshed = false;
	ECameraStatus m_cameraStatus = ECameraStatus::InShadow;
	Event OnExitedShadow;
	USkeletalMeshComponent* GetMesh() { return &mesh; }
	UCapsuleComponent* GetCapsuleComponent() { return &capsule; }
	template <class T> const T* FindComponentByClass() const { return &camera; }
	bool IsShadow() const { return shadow; }
	FVector GetFeetLocation() const { return feet; }
	void SetActorLocation(FVector value) { feet = {value.X, value.Y, value.Z - capsule.halfHeight}; }
	void RefreshInteractTarget() { refreshed = true; }
	void UpdateMeshCameraVisibility();
};
struct UPlayerStateBase
{
	std::function<void()> bpExit;
	void Exit() { if (bpExit) bpExit(); }
};
struct UPlayerShadowState : UPlayerStateBase
{
	using Super = UPlayerStateBase;
	APlayerCharacter* player = nullptr;
	bool bShadowApplied = true, bOriginalMeshVisible = true, bOriginalOverlapEvents = true, canRestore = true;
	float OriginalCapsuleRadius = 34, OriginalCapsuleHalfHeight = 88;
	int OriginalCollisionProfile = 1, OriginalMeshCollision = 1;
	FVector LastSafeFeet{100, 200, 0}, OriginalMeshLocation{0, 0, -88}, ExitShadowEffectOffset;
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;
	TArray<float> SinkMaterials, OriginalOpacities;
	TObjectPtr<int> ExitShadowEffectSystem;
	int stops = 0;
	APlayerCharacter* GetPlayer() { return player; }
	bool CanRestoreAtFeet(FVector) const { return canRestore; }
	void StopShadowEffect() { ++stops; }
	void SpawnTransitionEffect(int*, FVector) {}
	void Exit();
	void RestoreShadowState();
};

// PRODUCTION_METHODS

int main()
{
	for (bool canRestore : {false, true})
	{
		APlayerCharacter player;
		UPlayerShadowState state;
		state.player = &player;
		state.canRestore = canRestore;
		UMaterialInterface original, transparent;
		state.OriginalMaterials.push_back({&original});
		player.mesh.materials.push_back(&transparent);
		// BPで親を呼ばなくても復元し、Overlapから再度隠されても最後に戻す。
		state.bpExit = [&] { player.mesh.visible = false; };
		player.mesh.onOverlap = [&]
		{
			player.mesh.visible = false;
			player.mesh.hidden = true;
			player.mesh.materials[0] = &transparent;
		};
		player.OnExitedShadow.callback = [&]
		{
			assert(player.mesh.visible && !player.mesh.hidden);
			assert(player.mesh.materials[0] == &original);
			assert(player.mesh.position.Z == -88 && !player.mesh.bPauseAnims);
			assert(player.refreshed && player.mesh.overlap && player.mesh.collision == 1);
		};
		state.Exit();
		assert(player.OnExitedShadow.calls == 1 && state.stops == 1);
		assert(player.capsule.halfHeight == 88 && player.capsule.profile == 1);
		if (!canRestore) assert(player.feet.X == 100 && player.feet.Y == 200);
		assert(state.OriginalMaterials.Num() == 0);
		state.RestoreShadowState();
		assert(player.OnExitedShadow.calls == 1); // 二重復元・二重通知しない。
	}
	APlayerCharacter player;
	player.shadow = false;
	player.camera.position = {0, 0, 50};
	player.UpdateMeshCameraVisibility();
	assert(player.mesh.hidden); // 通常のカメラ接近時は従来通り隠す。
	player.m_bCameraSwitching = true;
	player.UpdateMeshCameraVisibility();
	assert(!player.mesh.hidden);
	player.m_bCameraSwitching = false;
	player.shadow = true;
	player.UpdateMeshCameraVisibility();
	assert(!player.mesh.hidden); // 影はVisibility側で管理し、HiddenInGameを残さない。
	player.shadow = false;
	player.camera.position = {0, 0, 300};
	player.UpdateMeshCameraVisibility();
	assert(!player.mesh.hidden);
	UPlayerShadowState neverEntered;
	neverEntered.player = &player;
	neverEntered.bShadowApplied = false;
	neverEntered.Exit();
	assert(player.OnExitedShadow.calls == 0);
}
