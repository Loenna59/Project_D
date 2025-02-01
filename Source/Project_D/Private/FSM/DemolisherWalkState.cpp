// Fill out your copyright notice in the Description page of Project Settings.

#include "FSM/DemolisherWalkState.h"

#include "BaseZombie.h"
#include "Demolisher.h"

void UDemolisherWalkState::OnEnter(class ABaseZombie* Zombie)
{
	if (!Zombie)
	{
		return;
	}
	
	CurrentDistance = Zombie->CalculateDistanceToTarget();
	CurrentTime = 0;

	Super::OnEnter(Zombie);
}

void UDemolisherWalkState::OnUpdate(class ABaseZombie* Zombie)
{
	Super::OnUpdate(Zombie);

	CurrentTime += GetWorld()->GetDeltaSeconds();
	
	if (Zombie)
	{
		if (ADemolisher* Demolisher = Cast<ADemolisher>(Zombie))
		{
			if (CurrentTime >= MaxWalkDuration)
			{
				Demolisher->Evaluate();
			}
			
			float Distance = Zombie->CalculateDistanceToTarget();
	
			float Destination = Demolisher->AttackRadius;
			
			if (CurrentDistance > Demolisher->MidRangeAttackRadius)
			{
				Destination = Demolisher->MidRangeAttackRadius;
			}
			
			if (Distance <= Destination)
			{
				Demolisher->Evaluate();
			}
		}
	}
}

void UDemolisherWalkState::OnExit(class ABaseZombie* Zombie)
{
	CurrentDistance = 0;
	
	Super::OnExit(Zombie);
}
