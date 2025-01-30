// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "GameDebug.h"
#include "Animation/ZombieAnimInstance.h"
#include "Pathfinding/PathfindingComponent.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->AnimationInstance)
		{
			Zombie->AnimationInstance->bIsWalking = true;
		}

		// if (AZombieAIController* AI = Cast<AZombieAIController>(Zombie->GetController()))
		// {
		// 	AI->MoveToTarget();
		// }

		Path = Zombie->Pathfinding->GetPaths(Zombie);
		Zombie->DistanceAlongSpline = 0.0f;
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->Pathfinding->UpdatePath())
		{
			Path = Zombie->Pathfinding->GetPaths(Zombie);
			Zombie->DistanceAlongSpline = 0.0f;
		}

		float DeltaTime = GetWorld()->DeltaTimeSeconds;

		if (Zombie->Pathfinding->MoveAlongSpline(Zombie, MovementSpeed, DeltaTime))
		{
			GameDebug::ShowDisplayLog(GetWorld(), TEXT("목표에 도달"));
		}
		
	}
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->AnimationInstance)
		{
			Zombie->AnimationInstance->bIsWalking = false;
		}

		Path.Empty();
		Zombie->DistanceAlongSpline = 0.0f;
	}
}
