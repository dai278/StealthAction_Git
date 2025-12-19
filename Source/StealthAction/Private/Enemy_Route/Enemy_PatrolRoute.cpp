// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Route/Enemy_PatrolRoute.h"

// Sets default values
AEnemy_PatrolRoute::AEnemy_PatrolRoute()
	: m_route_Num(0)
	, m_route_ActionOrder(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
