// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "Kismet/KismetSystemLibrary.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Enter");
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Update");
	}
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Exit");
	}
}
