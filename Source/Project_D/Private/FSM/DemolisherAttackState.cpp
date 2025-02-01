// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/DemolisherAttackState.h"

#include "BaseZombie.h"
#include "Demolisher.h"
#include "GameDebug.h"
#include "PhysicsHelper.h"
#include "Pathfinding/ZombieAIController.h"

void UDemolisherAttackState::OnEnter(ABaseZombie* Zombie)
{
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
	
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(),TEXT("ATTACK"));
		UpdateAttackPattern(Zombie);
	}
}

void UDemolisherAttackState::OnUpdate(ABaseZombie* Zombie)
{
	
}

void UDemolisherAttackState::OnExit(ABaseZombie* Zombie)
{
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
}

void UDemolisherAttackState::UpdateAttackPattern(ABaseZombie* Zombie)
{
	if (!Zombie)
	{
		return;
	}
	
	ADemolisher* Demolisher = Cast<ADemolisher>(Zombie);

	if (!Demolisher)
	{
		return;
	}

	// 공격 타입에 따라 지속 시간 설정
	float Duration = Interval;
	float DistanceToPlayer = Zombie->CalculateDistanceToTarget();
	
	if (DistanceToPlayer < 0)
	{
		return;
	}

	// 거리에 따라 공격 타입 선택
	if (DistanceToPlayer <= Zombie->AttackRadius)
	{
		Zombie->OnStartAttack();
	}
	else if (DistanceToPlayer <= Demolisher->MidRangeAttackRadius)
	{
		Demolisher->ChargeTo(ChargeSpeed, ChargeAcceleration);
		Duration = UPhysicsHelper::CalculateDuration(DistanceToPlayer, ChargeSpeed, ChargeAcceleration);
	}
	else
	{
		Duration = ThrowDuration;
		Demolisher->Throw();
	}

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[Zombie] ()
		{
			Zombie->FinishAttack();		
		},
		Duration,
		false
	);
}
