// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/DemolisherAttackState.h"

void UDemolisherAttackState::OnEnter(ABaseZombie* Zombie)
{
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
	
	if (Zombie)
	{
		// Zombie->b
	}
}

void UDemolisherAttackState::OnUpdate(ABaseZombie* Zombie)
{
}

void UDemolisherAttackState::OnExit(ABaseZombie* Zombie)
{
}
