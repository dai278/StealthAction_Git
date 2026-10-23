#include "State/Player/PlayerInteractState.h"
#include "State/Player/PlayerStateMachine.h"
#include "State/Player/PlayerInteractAnimationState.h"
#include "StealthAction/PlayerCharacter/PlayerCharacter.h"
#include "Interact/Interact.h"
#include "GameFramework/CharacterMovementComponent.h"

//-----------------------------------------------------
//コンストラクタ
//-----------------------------------------------------
UPlayerInteractState::UPlayerInteractState()
{
	PlayerStatus = EPlayerStatus::Interact;
	bAllowManualControl = false;
}

//-----------------------------------------------------
//開始条件
//-----------------------------------------------------
bool UPlayerInteractState::CanEnterState_Implementation() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	return PlayerCharacter && IsValid(PlayerCharacter->m_hitInteractOb) && PlayerCharacter->m_hitInteractOb->CanInteract();
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerInteractState::OnEnter_Implementation()
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->CancelDash(); }
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		InteractionTarget = PlayerCharacter->m_hitInteractOb;
		if (InteractionTarget.IsValid()) { InteractionPosition = InteractionTarget->GetInteractPosition(PlayerCharacter); }
	}
}

//-----------------------------------------------------
//更新処理
//-----------------------------------------------------
void UPlayerInteractState::OnUpdate_Implementation(float DeltaTime)
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	UPlayerStateMachine* Machine = GetPlayerMachine();
	if (!PlayerCharacter || !Machine) { return; }
	if (!InteractionTarget.IsValid()) { Machine->ReturnToLocomotion(); return; }
	FVector Direction = InteractionPosition - PlayerCharacter->GetActorLocation();
	Direction.Z = 0.f;
	if (InteractionPosition.IsNearlyZero() || Direction.Size() <= FMath::Max(0.f, AcceptanceRadius))
	{
		if (UPlayerInteractAnimationState* Animation = Machine->GetState<UPlayerInteractAnimationState>())
		{
			Animation->BeginInteraction(InteractionTarget.Get());
		}
		else { Machine->ChangePlayerState(UPlayerInteractAnimationState::StaticClass()); }
		return;
	}
	PlayerCharacter->AddMovementInput(Direction.GetSafeNormal(), FMath::Clamp(ApproachInputScale, 0.f, 1.f));
	PlayerCharacter->SetActorRotation(FMath::RInterpTo(PlayerCharacter->GetActorRotation(), Direction.Rotation(), DeltaTime, FMath::Max(0.f, RotationInterpSpeed)));
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerInteractState::OnExit_Implementation() { InteractionTarget.Reset(); }
