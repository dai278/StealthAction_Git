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
bool UPlayerInteractState::CanEnter() const
{
	APlayerCharacter* PlayerCharacter = GetPlayer();
	return PlayerCharacter && IsValid(PlayerCharacter->FindInteractTarget()) && Super::CanEnter();
}

//-----------------------------------------------------
//開始処理
//-----------------------------------------------------
void UPlayerInteractState::OnEnter_Implementation()
{
	if (UPlayerStateMachine* Machine = GetPlayerMachine()) { Machine->CancelDash(); }
	if (APlayerCharacter* PlayerCharacter = GetPlayer())
	{
		InteractionTarget = PlayerCharacter->FindInteractTarget();
		if (InteractionTarget.IsValid())
		{
			InteractionPosition = InteractionTarget->GetInteractPosition(PlayerCharacter);
			//接近不要を表す既存のゼロ指定も、対象があるときだけ受け付ける
			if (InteractionPosition.IsNearlyZero()) { InteractionPosition = PlayerCharacter->GetActorLocation(); }
		}
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
	if (!InteractionTarget.IsValid() || !PlayerCharacter->IsInInteractRange(InteractionTarget.Get()))
	{
		Machine->ReturnToLocomotion();
		return;
	}
	//動いている対象の操作位置も更新する
	const FVector CurrentPosition = InteractionTarget->GetInteractPosition(PlayerCharacter);
	if (!CurrentPosition.IsNearlyZero()) { InteractionPosition = CurrentPosition; }
	FVector Direction = InteractionPosition - PlayerCharacter->GetActorLocation();
	Direction.Z = 0.f;
	if (Direction.Size() <= FMath::Max(0.f, AcceptanceRadius))
	{
		if (UPlayerInteractAnimationState* Animation = Machine->GetState<UPlayerInteractAnimationState>())
		{
			if (!Animation->BeginInteraction(InteractionTarget.Get())) { Machine->ReturnToLocomotion(); }
		}
		else { Machine->ReturnToLocomotion(); }
		return;
	}
	PlayerCharacter->AddMovementInput(Direction.GetSafeNormal(), FMath::Clamp(ApproachInputScale, 0.f, 1.f));
	PlayerCharacter->SetActorRotation(FMath::RInterpTo(PlayerCharacter->GetActorRotation(), Direction.Rotation(), DeltaTime, FMath::Max(0.f, RotationInterpSpeed)));
}

//-----------------------------------------------------
//終了処理
//-----------------------------------------------------
void UPlayerInteractState::OnExit_Implementation() { InteractionTarget.Reset(); }
