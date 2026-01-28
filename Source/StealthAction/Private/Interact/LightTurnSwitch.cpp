// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/LightTurnSwitch.h"
#include "Light/ExtendedSpotLightManager.h"

//--------------------------
// コンストラクタ
//--------------------------
ALightTurnSwitch::ALightTurnSwitch()
{
}

//--------------------------
// ゲーム開始時に呼ばれる
//--------------------------
void ALightTurnSwitch::BeginPlay()
{
	Super::BeginPlay();

}

//--------------------------
// 毎フレーム呼ばれる
//--------------------------
void ALightTurnSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//--------------------------
// インタラクト処理
//--------------------------
void ALightTurnSwitch::Interact(AActor* _interactOwner)
{
	if (m_index == -1) { return; }
	UExtendedSpotLightManager* lightMng = GetWorld()->GetSubsystem<UExtendedSpotLightManager>();
	if (lightMng)
	{
		lightMng->OnRotateTargetAngleStop(m_index);
	}
}
