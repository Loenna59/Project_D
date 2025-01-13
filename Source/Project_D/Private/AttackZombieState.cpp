// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackZombieState.h"

#include "Kismet/GameplayStatics.h"

void UAttackZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Attack On Enter");
	}
}

void UAttackZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Attack On Update");
	}
}

void UAttackZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Attack On Exit");
	}
}
