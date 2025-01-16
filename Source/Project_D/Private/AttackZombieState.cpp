// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackZombieState.h"

#include "BaseZombie.h"
#include "Kismet/GameplayStatics.h"

void UAttackZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie && Zombie->DetectedTarget)
	{
		UKismetSystemLibrary::PrintString(GetWorld(),TEXT("ATTACK"));
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[Zombie] ()
			{
				Zombie->PlayAnimationMontage(EEnemyState::ATTACK);
			},
			Interval,
			true
		);
		Zombie->PlayAnimationMontage(EEnemyState::ATTACK);
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
