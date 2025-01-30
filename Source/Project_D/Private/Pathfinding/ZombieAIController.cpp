// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinding/ZombieAIController.h"

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AZombieAIController::SetTarget(AActor* Target)
{
	TargetActor = Target;
	if (TargetActor)
	{
		MoveToTarget();
	}
	else
	{
		StopMovement();
	}
}

void AZombieAIController::MoveToTarget()
{
	if (TargetActor)
	{
		MoveToActor(TargetActor, 100.f);
	}
}
