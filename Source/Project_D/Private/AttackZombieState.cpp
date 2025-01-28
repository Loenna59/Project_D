// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackZombieState.h"

#include "BaseZombie.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UAttackZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
	
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(),TEXT("ATTACK"));
		Zombie->OnTriggerAttack(true);
		
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[Zombie] ()
			{
				Zombie->OnTriggerAttack(false);
			},
			Interval,
			false
		);
	}
}

void UAttackZombieState::OnUpdate(ABaseZombie* Zombie)
{
	
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
