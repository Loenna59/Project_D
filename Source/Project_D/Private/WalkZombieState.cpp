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
		// UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Enter");
		// bool bIsSetupPathFinding = Zombie->StartPathfinding();
		Progress = 0.f;

		// if (Zombie->AnimationInstance)
		// {
		// 	Zombie->AnimationInstance->bIsWalking = bIsSetupPathFinding;
		// }

		if (AZombieAIController* AI = Cast<AZombieAIController>(Zombie->GetController()))
		{
			AI->MoveToTarget();
		}
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->AnimationInstance)
		{
			Zombie->AnimationInstance->bIsWalking = true;
		}
		
		Progress += GetWorld()->DeltaTimeSeconds * Zombie->GetCharacterMovement()->MaxWalkSpeed / 100.f;
		// Progress = Zombie->PlayPathfinding(Progress);
	}
	else
	{
		if (Zombie->AnimationInstance)
		{
			Zombie->AnimationInstance->bIsWalking = false;
		}
	}
	
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		Progress = 0.f;
	}
}
