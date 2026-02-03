// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interact/Interact.h"
#include "PasswordDoor.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHACTION_API APasswordDoor : public AInteract
{
	GENERATED_BODY()

public:
	APasswordDoor();

public:
	//文字追加
	void AddPasswordInput(const FString& input);

	//決定ボタン
	void confirmPassword();

private:
	//パスチェック
	bool checkPassword() const;

	//ドアを開ける
	void openDoor();

protected:
	virtual void Interact(AActor* _interactOwner) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Door")
	UStaticMeshComponent* m_pDoorMesh;		//ドアメッシュ

	UPROPERTY(EditAnywhere, Category = "Password")
	FString m_password;						//パスワード

	FString m_inputPassword;				//入力中のパスワード

	bool m_bIsOpen;							//ドアが開いているか
	
};
