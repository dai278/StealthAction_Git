#include "State/Player/PlayerDashState.h"
#include "State/Player/PlayerStateMachine.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerDashState::UPlayerDashState()
{
	bOverrideCamera = true;
	CameraSettings = { FRotator(0.f, 0.f, 0.f), 90.f, 200.f, FVector(0.f, 70.f, 20.f), 5.f };
	MoveSpeed = 600.f;
	StaminaConsumptionRate = 1.f;
}

//-----------------------------------------------------
//開始条件
//-----------------------------------------------------
bool UPlayerDashState::CanEnterState_Implementation() const
{
	const UPlayerStateMachine* Machine = GetPlayerMachine();
	return Machine && !Machine->IsStaminaDepleted();
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerDashState::OnEnter_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		PlayerCharacter->m_cameraStatus = ECameraStatus::Dash;
		PlayerCharacter->m_bCameraSwitching = true;
	}
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerDashState::OnExit_Implementation()
{
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		PlayerCharacter->m_cameraStatus = ECameraStatus::ThirdPerson;
		PlayerCharacter->m_bCameraSwitching = true;
	}
}
