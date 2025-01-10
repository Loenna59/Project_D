// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackZombieState.h"

void UAttackZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack On Enter"));
	}
}

void UAttackZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack On Update"));
	}
}

void UAttackZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack On Exit"));
	}
}
