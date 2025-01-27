// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "BiterAnimInstance.h"
#include "Animation/ZombieAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
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
			AI->MoveToTarget();
		}
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->AnimationInstance)
		{
			Zombie->AnimationInstance->bIsWalking = true;
		}
	}
}
