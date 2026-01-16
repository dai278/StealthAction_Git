#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDamage.generated.h"

UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

class STEALTHACTION_API IDamageable
{
	GENERATED_BODY()

public:
	// ƒˆ‰¼‘zŠÖ”iBlueprint”ñ‘Î‰j
	virtual void OnDamage(int32 Damage, FVector KnockBackVec, bool bSneakKill) = 0;
};