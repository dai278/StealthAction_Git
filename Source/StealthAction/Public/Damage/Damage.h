// Damageable.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damage.generated.h"

UINTERFACE(Blueprintable)
class STEALTHACTION_API UDamageable : public UInterface
{
    GENERATED_BODY()
};

class STEALTHACTION_API IDamageable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnDamage(int32 Damage, float KnockBackValue, bool bSneakKill);
};
