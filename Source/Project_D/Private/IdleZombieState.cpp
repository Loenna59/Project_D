// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleZombieState.h"

#include "BaseZombie.h"
#include "BiterAnimInstance.h"
#include "Animation/ZombieAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UIdleZombieState::OnEnter(class ABaseZombie* Zombie)
{
	if (Zombie && Zombie->AnimationInstance)
	{
		Zombie->AnimationInstance->bIsWalking = false;
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
