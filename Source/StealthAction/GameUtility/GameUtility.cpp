

//インクルー度
#include "GameUtility.h"
#include "StealthAction/MyWidget.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Containers/UnrealString.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"

//タグからActorを取得
AActor* CGameUtility::GetActorFromTag(AActor* _pOwnerActor, const FName& _tag) {
	if (_pOwnerActor == nullptr) {
		return nullptr;
	}

	//ゲーム全体に対するActorに検索処理は重いため
	TSubclassOf<AActor>findClass;
	findClass = AActor::StaticClass();
	TArray<AActor*>actors;
	UGameplayStatics::GetAllActorsOfClass(_pOwnerActor->GetWorld(), findClass, actors);

	//検索結果Actorがあれば
	if (actors.Num() > 0) {
		//そのアクタの順番に検索
		for (int i = 0; i < actors.Num(); i++)
		{
			AActor* pActor = Cast<AActor>(actors[i]);
			//タグ名で判別
			if (pActor->ActorHasTag(_tag)) {
				return pActor;
			}
		}
	}
	return nullptr;
}

//処理落ち補正値の取得
float CGameUtility::GetFpsCorrection(float _deltaTiem)
{
	//現在のfps測定
	float fps = 1.0f / _deltaTiem;

	return (60.0f / fps);
}