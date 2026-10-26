#include <algorithm>
#include <cassert>
#include <cmath>

struct FVector
{
	double X = 0, Y = 0, Z = 0;
	FVector() = default;
	FVector(double x, double y, double z) : X(x), Y(y), Z(z) {}
	FVector operator+(FVector b) const { return {X+b.X, Y+b.Y, Z+b.Z}; }
	FVector operator-(FVector b) const { return {X-b.X, Y-b.Y, Z-b.Z}; }
};
struct FMath
{
	template <class T> static T Max(T a, T b) { return std::max(a,b); }
	template <class T> static T Clamp(T a, T low, T high) { return std::clamp(a,low,high); }
};
template <class T> bool IsValid(T* value) { return value != nullptr; }
struct UPlayerStateBase
{
	virtual ~UPlayerStateBase() = default;
	template <class T> bool IsA() const { return dynamic_cast<const T*>(this) != nullptr; }
};
struct UPlayerStateMachine
{
	const UPlayerStateBase* target = nullptr;
	bool dash = true;
	void CancelDash() { dash = false; }
	const UPlayerStateBase* GetTransitionTarget() const { return target; }
};
struct UCapsuleComponent
{
	float halfHeight = 80, radius = 34, scale = 1;
	int profile = 1;
	float GetScaledCapsuleHalfHeight() const { return halfHeight * scale; }
	float GetUnscaledCapsuleHalfHeight() const { return halfHeight; }
	float GetUnscaledCapsuleRadius() const { return radius; }
	void SetCapsuleHalfHeight(float height) { halfHeight = std::max(radius, height); }
	void SetCapsuleSize(float r, float h) { radius = r; halfHeight = std::max(r,h); }
	int GetCollisionProfileName() const { return profile; }
	void SetCollisionProfileName(int value) { profile = value; }
};
struct USkeletalMeshComponent
{
	FVector world;
	void AddWorldOffset(FVector offset) { world = world + offset; }
	FVector GetRelativeLocation() const { return world; }
};
enum class ECameraStatus { ThirdPerson, Crouch };
struct APlayerCharacter
{
	UCapsuleComponent capsule;
	USkeletalMeshComponent mesh;
	FVector location{0,0,180}, m_saveLastNotUpWallPos;
	float m_capsuleHeight = 80;
	bool blockedAbove = false, m_bCameraSwitching = false;
	ECameraStatus m_cameraStatus = ECameraStatus::ThirdPerson;
	UCapsuleComponent* GetCapsuleComponent() { return &capsule; }
	USkeletalMeshComponent* GetMesh() { return &mesh; }
	FVector GetActorLocation() const { return location; }
	FVector GetFeetLocation() const { return location - FVector(0,0,capsule.GetScaledCapsuleHalfHeight()); }
	bool IsUpPosWall(FVector) const { return blockedAbove; }
	void SetActorLocation(FVector target) { mesh.world = mesh.world + (target-location); location = target; }
};
struct UPlayerCrouchState : UPlayerStateBase
{
	APlayerCharacter* player = nullptr;
	UPlayerStateMachine* machine = nullptr;
	float CapsuleHeightRatio = .8f;
	APlayerCharacter* GetPlayer() const { return player; }
	UPlayerStateMachine* GetPlayerMachine() const { return machine; }
	void OnEnter_Implementation();
	void OnExit_Implementation();
	void ResizeCapsuleAtFeet(float, const FVector&);
};
struct UPlayerShadowState : UPlayerStateBase
{
	int OriginalCollisionProfile = 0, ShadowCollisionProfile = 2;
	float ShadowCapsuleRadius = 10, ShadowCapsuleHalfHeight = 10;
	FVector SinkStartMeshLocation;
	FVector LastSafeFeet;
	bool restoreAllowed = true;
	bool CanRestoreAtFeet(FVector) const { return restoreAllowed; }
	void EnterGeometry(APlayerCharacter* PlayerCharacter)
	{
		UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();
		USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh();
		const float PreviousHeight = Capsule->GetScaledCapsuleHalfHeight();
		LastSafeFeet = PlayerCharacter->GetFeetLocation();
		// PRODUCTION_SHADOW_GEOMETRY
	}
};

// PRODUCTION_METHODS

void Check(FVector a, FVector b)
{
	assert(std::abs(a.X-b.X)<.0001 && std::abs(a.Y-b.Y)<.0001 && std::abs(a.Z-b.Z)<.0001);
}

int main()
{
	for (float scale : {1.f, 1.5f, 2.f})
	{
		for (float ratio : {.8f, .2f, 1.f})
		{
			APlayerCharacter player;
			player.capsule.scale = scale;
			player.location = {20,30,100+80*scale};
			player.mesh.world = {20,30,100};
			player.m_saveLastNotUpWallPos = {700,800,100+80*scale};
			const FVector feet = player.GetFeetLocation();
			const FVector meshLocation = player.mesh.world;
			UPlayerStateMachine machine;
			UPlayerCrouchState crouch;
			crouch.player = &player;
			crouch.machine = &machine;
			crouch.CapsuleHeightRatio = ratio;
			crouch.OnEnter_Implementation();
			assert(!machine.dash);
			Check(player.GetFeetLocation(), feet); // 影判定で使う足元が浮かない。
			Check(player.mesh.world, meshLocation);
			crouch.OnExit_Implementation();
			Check(player.GetFeetLocation(), feet); // 立つ時も床に広がらない。
			Check(player.mesh.world, meshLocation);

			// 低い天井下でも、影へは退避先へワープせず同じ足元から入る。
			crouch.OnEnter_Implementation();
			player.blockedAbove = true;
			UPlayerShadowState shadow;
			shadow.restoreAllowed = false;
			machine.target = &shadow;
			crouch.OnExit_Implementation();
			Check(player.GetFeetLocation(), feet);
			shadow.EnterGeometry(&player);
			Check(player.GetFeetLocation(), feet);
			Check(player.mesh.world, meshLocation);
			Check(shadow.LastSafeFeet, player.m_saveLastNotUpWallPos - FVector(0,0,80*scale));
			assert(player.capsule.profile==2 && player.capsule.halfHeight==10);

			// 通常状態への復帰時は、保存済みの立ち位置へ戻す。
			machine.target = nullptr;
			crouch.OnExit_Implementation();
			Check(player.location, player.m_saveLastNotUpWallPos);
			assert(player.capsule.halfHeight==80);
		}
	}
}
