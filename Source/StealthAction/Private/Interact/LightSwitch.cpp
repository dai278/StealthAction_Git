#include "Interact/LightSwitch.h"

#include "Light/ExtendedSpotLightManager.h"

#include "Components/BoxComponent.h"

//--------------------------
// コンストラクタ
//--------------------------
ALightSwitch::ALightSwitch()
{
	InteractPos[0] = GetActorLocation();

}

//--------------------------
// ゲーム開始時に呼ばれる
//--------------------------
void ALightSwitch::BeginPlay()
{
	Super::BeginPlay();


}

//--------------------------
// 毎フレーム呼ばれる
//--------------------------
void ALightSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//--------------------------
// インタラクト処理
//--------------------------
void ALightSwitch::Interact(AActor* _interactOwner)
{
	if (m_indexs.Num()<=0) { return; }
	UExtendedSpotLightManager* lightMng = GetWorld()->GetSubsystem<UExtendedSpotLightManager>();
	if (lightMng)
	{
		for (int32 index : m_indexs)
		{
			lightMng->SwitchTurnLight(index);

		}
	}
}
