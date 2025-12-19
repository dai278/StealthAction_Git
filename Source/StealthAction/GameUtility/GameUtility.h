#pragma once




class FString;
class UTestWidget;
class AActor;

class  CGameUtility
{
public:

	//ƒ^ƒO‚©‚çActor‚ğæ“¾
	static AActor* GetActorFromTag(AActor* _pOwnerActor, const FName& _tag);

	//ˆ——‚¿•â³’l‚ğæ“¾
	static float GetFpsCorrection(float _deltaTime);
};


