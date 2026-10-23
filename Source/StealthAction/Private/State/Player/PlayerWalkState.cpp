#include "State/Player/PlayerWalkState.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerWalkState::UPlayerWalkState() {
	PlayerStatus = EPlayerStatus::Walk;
	MoveSpeed = 300.f;
}
