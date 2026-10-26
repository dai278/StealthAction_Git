#include <algorithm>
#include <cassert>
#include <cmath>

struct FVector
{
	double X = 0, Y = 0, Z = 0;
	FVector() = default;
	FVector(double x, double y, double z) : X(x), Y(y), Z(z) {}
	static const FVector ZeroVector;
	static double DotProduct(FVector a, FVector b) { return a.X * b.X + a.Y * b.Y + a.Z * b.Z; }
	FVector operator+(FVector b) const { return {X + b.X, Y + b.Y, Z + b.Z}; }
	FVector operator-(FVector b) const { return {X - b.X, Y - b.Y, Z - b.Z}; }
	FVector operator*(double n) const { return {X * n, Y * n, Z * n}; }
	bool IsNearlyZero() const { return X * X + Y * Y + Z * Z < 1e-8; }
	FVector GetSafeNormal() const
	{
		const double length = std::sqrt(X * X + Y * Y + Z * Z);
		return length > 1e-8 ? *this * (1 / length) : ZeroVector;
	}
};
const FVector FVector::ZeroVector{};

struct FTransform
{
	FVector origin, scale{1, 1, 1};
	double yaw = 0;
	FVector TransformVector(FVector v) const
	{
		v = {v.X * scale.X, v.Y * scale.Y, v.Z * scale.Z};
		return {std::cos(yaw) * v.X - std::sin(yaw) * v.Y,
			std::sin(yaw) * v.X + std::cos(yaw) * v.Y, v.Z};
	}
	FVector InverseTransformPosition(FVector v) const
	{
		v = v - origin;
		return {(std::cos(yaw) * v.X + std::sin(yaw) * v.Y) / scale.X,
			(-std::sin(yaw) * v.X + std::cos(yaw) * v.Y) / scale.Y, v.Z / scale.Z};
	}
};

struct FMath
{
	static float Min(float a, float b) { return std::min(a, b); }
	template <class T> static T Max(T a, T b) { return std::max(a, b); }
	static float Abs(float a) { return std::abs(a); }
	static FVector Lerp(FVector a, FVector b, float alpha) { return a + (b - a) * alpha; }
};
struct FHitResult { bool bBlockingHit = false; };
struct AActor
{
	FVector location;
	FVector forward{1, 0, 0};
	FVector GetActorLocation() const { return location; }
	FVector GetActorForwardVector() const { return forward; }
};
struct UBoxComponent
{
	FTransform transform;
	FVector extent{50, 50, 50};
	FTransform GetComponentTransform() const { return transform; }
	FVector GetUnscaledBoxExtent() const { return extent; }
};
template <class T> bool IsValid(T* value) { return value != nullptr; }
struct AInteract : AActor { void Tick(float) {} };
struct APushMoveActor : AInteract
{
	using Super = AInteract;
	UBoxComponent body;
	UBoxComponent* m_bodyCollision = &body;
	bool m_bIsMoving = false, tickEnabled = false, blocked = false;
	float m_moveDistance = 100, m_moveSpeed = 100, m_moveProgress = 0;
	FVector m_moveDirection, m_startLocation, m_targetLocation;
	int moveCalls = 0;
	void SetActorTickEnabled(bool enabled) { tickEnabled = enabled; }
	void SetActorLocation(FVector value, bool, FHitResult* hit)
	{
		++moveCalls;
		hit->bBlockingHit = blocked;
		if (!blocked) location = value;
	}
	void Interact(AActor*);
	void Tick(float);
};

// PRODUCTION_METHODS

void Check(FVector a, FVector b) { assert((a - b).IsNearlyZero()); }

int main()
{
	// 同じ面なら、配置位置・回転・スケールが変わっても面の反対側へ押す。
	for (double yaw : {0., .7, 1.5707963267948966, 3.141592653589793})
	{
		for (FVector scale : {FVector{1, 1, 1}, FVector{2, .5, 1}, FVector{-1, 2, 1}})
		{
			for (FVector side : {FVector{1, 0, 0}, FVector{-1, 0, 0}, FVector{0, 1, 0}, FVector{0, -1, 0}})
			{
				APushMoveActor box;
				box.location = {1300, -850, 70};
				box.body.transform = {box.location, scale, yaw};
				AActor player;
				player.location = box.location + box.body.transform.TransformVector(side * 100) + FVector{0, 0, 90};
				box.Interact(&player);
				assert(box.m_bIsMoving && box.tickEnabled);
				Check(box.m_moveDirection, box.body.transform.TransformVector(side * -1).GetSafeNormal());
				const FVector target = box.m_targetLocation;
				player.location = box.location - (player.location - box.location);
				box.Interact(&player);
				Check(box.m_targetLocation, target); // 移動中は方向を変えない。
				box.Tick(10);
				Check(box.location, target); // 低fpsでも通り越さない。
				assert(!box.m_bIsMoving && !box.tickEnabled && box.moveCalls == 1);
			}
		}
	}
	// 四隅では位置のわずかな差より向きを優先し、斜めには押さない。
	for (double yaw : {0., .7, 1.5707963267948966})
	{
		for (FVector scale : {FVector{1, 1, 1}, FVector{2, .5, 1}, FVector{-1, 2, 1}})
		{
			for (double xSign : {-1., 1.})
			{
				for (double ySign : {-1., 1.})
				{
					for (bool useX : {false, true})
					{
						for (double offset : {-.1, 0., .1})
						{
							APushMoveActor corner;
							corner.location = {900, -300, 50};
							corner.body.extent = {200, 50, 50};
							corner.body.transform = {corner.location, scale, yaw};
							const auto transform = corner.body.transform;
							const FVector pushX = transform.TransformVector({-xSign, 0, 0}).GetSafeNormal();
							const FVector pushY = transform.TransformVector({0, -ySign, 0}).GetSafeNormal();
							const FVector expected = useX ? pushX : pushY;
							AActor pusher;
							pusher.location = corner.location + transform.TransformVector({xSign * (220 + offset), ySign * 70, 90});
							pusher.forward = (expected + (useX ? pushY : pushX) * .2).GetSafeNormal();
							corner.Interact(&pusher);
							assert(corner.m_bIsMoving);
							Check(corner.m_moveDirection, expected);
						}
					}
				}
			}
		}
	}
	APushMoveActor away;
	AActor facingAway;
	facingAway.location = {80, 80, 90};
	facingAway.forward = {1, 1, 0};
	away.Interact(&facingAway);
	assert(!away.m_bIsMoving);

	APushMoveActor wide;
	wide.body.extent = {200, 50, 50};
	AActor player;
	player.location = {150, 80, 90};
	wide.Interact(&player);
	Check(wide.m_moveDirection, {0, -1, 0}); // 中心の角度ではX方向に誤判定していた位置。
	wide.blocked = true;
	wide.Tick(.5);
	Check(wide.location, FVector::ZeroVector);
	assert(!wide.tickEnabled && !wide.m_bIsMoving);

	APushMoveActor invalid;
	invalid.Interact(nullptr);
	player.forward = {-1, 0, 0};
	invalid.Interact(&player); // 正常に開始できる。
	assert(invalid.m_bIsMoving);
	APushMoveActor zero;
	player.location = {};
	zero.Interact(&player);
	assert(!zero.m_bIsMoving); // 真上では方向を決めない。
	player.location = {100, 0, 0};
	zero.m_moveDistance = 0;
	zero.Interact(&player);
	assert(!zero.m_bIsMoving);
	zero.m_moveDistance = 100;
	zero.m_moveSpeed = 0;
	zero.Interact(&player);
	assert(!zero.m_bIsMoving);
}
