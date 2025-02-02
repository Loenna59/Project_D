// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackZombieState.h"

#include "BaseZombie.h"

void UAttackZombieState::Initialize(const float _Interval)
{
	Interval = _Interval;
}

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
		Zombie->OnStartAttack();
		
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[Zombie] ()
			{
				Zombie->FinishAttack();		
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
