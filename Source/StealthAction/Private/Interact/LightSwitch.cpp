//’S“–“úîà
#include "Interact/LightSwitch.h"
#include "Engine/World.h"

#include "Light/ExtendedSpotLightManager.h"

#include "Components/BoxComponent.h"

//--------------------------
// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
//--------------------------
ALightSwitch::ALightSwitch()
{
	InteractPos[0] = GetActorLocation();

}

//--------------------------
// ƒQ[ƒ€ŠJn‚ÉŒÄ‚Î‚ê‚é
//--------------------------
void ALightSwitch::BeginPlay()
{
	Super::BeginPlay();


}

//--------------------------
// –ˆƒtƒŒ[ƒ€ŒÄ‚Î‚ê‚é
//--------------------------
void ALightSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//--------------------------
// ƒCƒ“ƒ^ƒ‰ƒNƒgˆ—
//--------------------------
void ALightSwitch::Interact(AActor* _interactOwner)
{
	if (m_indexs.Num()<=0) { return; }
	UExtendedSpotLightManager* lightMng = (GetWorld() ? GetWorld()->GetSubsystem<UExtendedSpotLightManager>() : nullptr);
	if (lightMng)
	{
		for (int32 index : m_indexs)
		{
			lightMng->SwitchTurnLight(index);

		}
	}
}


//--------------------------
// ƒCƒ“ƒ^ƒ‰ƒNƒgˆ—iƒ|[ƒY‚·‚é‚©‚Ç‚¤‚©j
//--------------------------
void ALightSwitch::Interact(AActor* _interactOwner, bool& _isPose)
{
	Interact(_interactOwner);
	_isPose = false;
}