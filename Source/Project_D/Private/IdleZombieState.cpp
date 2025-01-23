// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleZombieState.h"

#include "BaseZombie.h"
#include "BiterAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UIdleZombieState::OnEnter(class ABaseZombie* Zombie)
{
	if (Zombie)
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

void UIdleZombieState::OnUpdate(class ABaseZombie* Zombie)
{
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "Idle On Update");
	}
}

void UIdleZombieState::OnExit(class ABaseZombie* Zombie)
{
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "Idle On Exit");
	}
}
