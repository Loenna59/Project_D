// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackZombieState.h"

#include "BaseZombie.h"
#include "Kismet/GameplayStatics.h"

void UAttackZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
	
	if (Zombie && Zombie->DetectedTarget)
	{
		UKismetSystemLibrary::PrintString(GetWorld(),TEXT("ATTACK"));
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[Zombie] ()
			{
				Zombie->OnTriggerAttack(false);
			},
			Interval,
			false
		);
		Zombie->OnTriggerAttack(true);
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
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
	
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "Attack On Exit");
	}
}
