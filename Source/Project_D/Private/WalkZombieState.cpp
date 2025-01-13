// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Walk On Enter"));
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Walk On Update"));
	}
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Walk On Exit"));
	}
}
