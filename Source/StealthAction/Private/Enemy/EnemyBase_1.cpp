// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase_1.h"
#include "Enemy/Enemy_1.h"

UEnemyBase_1::UEnemyBase_1()
{

}

void UEnemyBase_1::UpdateVisiblity(ACharacter* m_pEnemy)
{

	AEnemy_1* Enemy = Cast<AEnemy_1>(m_pEnemy);

	UE_LOG(LogTemp, Warning, TEXT("Vision = %d"), Enemy->m_visionLevel);
}