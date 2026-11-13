#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

#define SCENE_QUERY_STAT(Name) 0
enum { ECC_WorldStatic, ECC_WorldDynamic, ECC_GameTraceChannel8 };
struct FVector
{
	double X = 0, Y = 0, Z = 0;
	FVector operator-(FVector b) const { return {X-b.X,Y-b.Y,Z-b.Z}; }
	double Size() const { return std::sqrt(X*X+Y*Y+Z*Z); }
	void Normalize() { const double n=Size(); if(n>0) { X/=n; Y/=n; Z/=n; } }
	static double DotProduct(FVector a,FVector b) { return a.X*b.X+a.Y*b.Y+a.Z*b.Z; }
};
struct FMath
{
	static double Cos(double n) { return std::cos(n); }
	static double DegreesToRadians(double n) { return n*3.141592653589793/180.; }
};
struct UPrimitiveComponent { virtual ~UPrimitiveComponent()=default; };
struct UStaticMeshComponent : UPrimitiveComponent {};
struct USkeletalMeshComponent : UPrimitiveComponent {};
template <class T,class U> T* Cast(U* p) { return dynamic_cast<T*>(p); }
template <class T> bool IsValid(T* p) { return p!=nullptr; }
struct FHitResult
{
	UPrimitiveComponent* component=nullptr;
	float Distance=0;
	UPrimitiveComponent* GetComponent() const { return component; }
};
struct FCollisionObjectQueryParams
{
	std::vector<int> types;
	void AddObjectTypesToQuery(int type) { types.push_back(type); }
};
struct FCollisionQueryParams
{
	bool complex;
	std::vector<UPrimitiveComponent*> ignored;
	FCollisionQueryParams(int,bool flag):complex(flag) {}
	void AddIgnoredActor(const void*) {}
	void AddIgnoredComponent(UPrimitiveComponent* component) { ignored.push_back(component); }
};
struct USpotLightComponent
{
	float AttenuationRadius=100, OuterConeAngle=45;
	FVector GetComponentLocation() const { return {}; }
	FVector GetForwardVector() const { return {1,0,0}; }
};
struct World
{
	std::vector<FHitResult> hits;
	int calls=0;
	bool lastComplex=false;
	bool LineTraceSingleByObjectType(FHitResult& hit,FVector,FVector,const FCollisionObjectQueryParams& objects,const FCollisionQueryParams& params)
	{
		assert(objects.types.size()==3);
		++calls;
		lastComplex=params.complex;
		assert(calls<20);
		for(const auto& candidate:hits)
		{
			if(std::find(params.ignored.begin(),params.ignored.end(),candidate.component)==params.ignored.end())
			{
				hit=candidate;
				return true;
			}
		}
		return false;
	}
};
struct AExtendedSpotLight
{
	World* world=nullptr;
	USpotLightComponent* m_pSpotLight=nullptr;
	bool m_bIsOn=true, bTraceComplexForOcclusion=false;
	World* GetWorld() const { return world; }
	bool IsHit(const FVector&) const;
};

// PRODUCTION_METHOD

int main()
{
	World world;
	USpotLightComponent spot;
	AExtendedSpotLight light;
	light.world=&world;
	light.m_pSpotLight=&spot;
	UPrimitiveComponent capsule,box;
	UStaticMeshComponent mesh;
	USkeletalMeshComponent enemy;
	const FVector feet{80,0,0};
	assert(light.IsHit(feet) && !world.lastComplex);
	world.hits={{&capsule,10},{&box,15}};
	world.calls=0;
	assert(light.IsHit(feet) && world.calls==3);
	world.hits.push_back({&mesh,30});
	world.calls=0;
	assert(!light.IsHit(feet) && world.calls==3);
	world.hits={{&capsule,10},{&enemy,30}};
	world.calls=0;
	assert(!light.IsHit(feet) && world.calls==2);
	world.hits={{&mesh,80}};
	world.calls=0;
	assert(light.IsHit(feet));
	light.bTraceComplexForOcclusion=true;
	assert(light.IsHit(feet) && world.lastComplex);
	const int calls=world.calls;
	assert(!light.IsHit({200,0,0}));
	assert(!light.IsHit({0,80,0}));
	light.m_bIsOn=false;
	assert(!light.IsHit(feet) && world.calls==calls);
}
