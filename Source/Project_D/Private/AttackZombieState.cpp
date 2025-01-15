// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackZombieState.h"

#include "BaseZombie.h"
#include "Kismet/GameplayStatics.h"

void UAttackZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie && Zombie->DetectedTarget)
	{
		UKismetSystemLibrary::PrintString(GetWorld(),TEXT("ATTACK"));
	}
}

void UAttackZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		
	}
}

void UAttackZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "Attack On Exit");
	}
}
