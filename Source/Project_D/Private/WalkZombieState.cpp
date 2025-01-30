// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "Animation/ZombieAnimInstance.h"
#include "Pathfinding/PathfindingComponent.h"
#include "Pathfinding/ZombieAIController.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->AnimationInstance)
		{
			Zombie->AnimationInstance->bIsWalking = true;
		}
		
		if (AZombieAIController* AI = Cast<AZombieAIController>(Zombie->GetController()))
		{
			Zombie->Pathfinding->GetPaths(Zombie);
			AI->SetSplinePoint(Zombie->Pathfinding->SplineComponent);
			AI->MoveAlongSpline();
		}
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->Pathfinding->UpdatePath())
		{
			if (AZombieAIController* AI = Cast<AZombieAIController>(Zombie->GetController()))
			{
				Zombie->Pathfinding->GetPaths(Zombie);
				AI->SetSplinePoint(Zombie->Pathfinding->SplineComponent);
			}
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

		if (AZombieAIController* AI = Cast<AZombieAIController>(Zombie->GetController()))
		{
			AI->StopMovement();
		}
	}
}
