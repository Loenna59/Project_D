// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleZombieState.h"

void UIdleZombieState::OnEnter(class ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Idle On Enter"));
	}
}

void UIdleZombieState::OnUpdate(class ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Idle On Update"));
	}
}

void UIdleZombieState::OnExit(class ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Idle On Exit"));
	}
}
