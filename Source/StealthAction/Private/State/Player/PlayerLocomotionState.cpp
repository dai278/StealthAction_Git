#include "State/Player/PlayerLocomotionState.h"
#include "State/Player/PlayerStateMachine.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerLocomotionState::UPlayerLocomotionState()
{
	bAllowMovementInput = true;
	FootstepNoiseLevel = 1;
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerLocomotionState::OnUpdate_Implementation(float DeltaTime)
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->RefreshLocomotionState(); }
}
