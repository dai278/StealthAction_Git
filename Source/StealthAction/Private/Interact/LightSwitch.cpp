#include "Interact/LightSwitch.h"

#include "Light/ExtendedSpotLightManager.h"

#include "Components/BoxComponent.h"
//--------------------------
// コンストラクタ
//--------------------------
ALightSwitch::ALightSwitch()
	:m_lightIndex(-1)
{
	//コリジョン生成
	m_pCollision = CreateDefaultSubobject<UBoxComponent> (TEXT("m_pCollisionBox"));
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pMesh"));

	//if (m_pCollision && m_pMesh)
	//{

	//	//メッシュをルートコンポーネントに設定
	//	m_pMesh->SetupAttachment(GetRootComponent());
	//	m_pCollision->SetupAttachment(m_pMesh);

	//	m_pMesh->SetRelativeLocation(FVector::ZeroVector);
	//	m_pMesh->SetRelativeRotation(FRotator::ZeroRotator);

	//	m_pCollision->SetRelativeLocation(FVector::ZeroVector);
	//	m_pCollision->SetRelativeRotation(FRotator::ZeroRotator);
	//}
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
