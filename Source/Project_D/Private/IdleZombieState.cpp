// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleZombieState.h"

#include "Kismet/KismetSystemLibrary.h"

void UIdleZombieState::OnEnter(class ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Idle On Enter");
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
		UKismetSystemLibrary::PrintString(GetWorld(), "Idle On Exit");
	}
}
