#include "Interact/LightSwitch.h"

#include "Light/ExtendedSpotLightManager.h"
//--------------------------
// コンストラクタ
//--------------------------
ALightSwitch::ALightSwitch()
	:m_lightIndex(-1)
{
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
void ALightSwitch::Interact()
{
	if (m_lightIndex == -1) { return; }
	UExtendedSpotLightManager* lightMng = GetWorld()->GetSubsystem<UExtendedSpotLightManager>();
	if (lightMng)
	{
		lightMng->SwitchTurnLight(m_lightIndex);
	}
}
