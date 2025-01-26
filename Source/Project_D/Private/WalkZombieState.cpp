// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "BiterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Enter");
		bool bIsSetupPathFinding = Zombie->StartPathfinding();
		Progress = 0.f;

		if (UAnimInstance* const Anim = Zombie->GetMesh()->GetAnimInstance())
		{
			if (UBiterAnimInstance* BiterAnimInstance = Cast<UBiterAnimInstance>(Anim))
			{
				BiterAnimInstance->bIsWalking = bIsSetupPathFinding;
			}
		}
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (UAnimInstance* const Anim = Zombie->GetMesh()->GetAnimInstance())
		{
			if (UBiterAnimInstance* BiterAnimInstance = Cast<UBiterAnimInstance>(Anim))
			{
				BiterAnimInstance->bIsWalking |= true;
			}
		}
		
		Progress += GetWorld()->GetDeltaSeconds() * Zombie->GetCharacterMovement()->MaxWalkSpeed / 100.f;
		Progress = Zombie->PlayPathfinding(Progress);
	}
	else
	{
		if (UAnimInstance* const Anim = Zombie->GetMesh()->GetAnimInstance())
		{
			if (UBiterAnimInstance* BiterAnimInstance = Cast<UBiterAnimInstance>(Anim))
			{
				BiterAnimInstance->bIsWalking = false;
			}
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
