#include "State/Player/PlayerIdleState.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerIdleState::UPlayerIdleState() {
	PlayerStatus = EPlayerStatus::Idle;
	MoveSpeed = 300.f;
}
