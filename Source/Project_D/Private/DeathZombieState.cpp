// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathZombieState.h"

#include "BaseZombie.h"
#include "Kismet/KismetSystemLibrary.h"

void UDeathZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Dead");
		Zombie->OnDisbale();
	}
}

void UDeathZombieState::OnUpdate(ABaseZombie* Zombie)
{
}

void UDeathZombieState::OnExit(ABaseZombie* Zombie)
{
}
