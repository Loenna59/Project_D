// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/DemolisherAttackState.h"

#include "BaseZombie.h"
#include "Demolisher.h"
#include "PhysicsHelper.h"

void UDemolisherAttackState::Initialize(
	const float _Interval,
	const float _ThrowDuration,
	const float _ChargeSpeed,
	const float _ChargeAcceleration
)
{
	Interval = _Interval;
	ThrowDuration = _ThrowDuration;
	ChargeSpeed = _ChargeSpeed;
	ChargeAcceleration = _ChargeAcceleration;
}

void UDemolisherAttackState::OnEnter(ABaseZombie* Zombie)
{
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
	
	if (Zombie)
	{
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
	
	TWeakObjectPtr<ADemolisher> Demolisher = Cast<ADemolisher>(Zombie);

	if (!Demolisher.IsValid())
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
		Duration = UPhysicsHelper::CalculateDuration(DistanceToPlayer, ChargeSpeed, ChargeAcceleration) * 0.667f;
	}
	else
	{
		Duration = ThrowDuration;
		Demolisher->Throw();
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[Demolisher] ()
		{
			if (!Demolisher.IsValid())
			{
				return;
			}
			
			Demolisher->FinishAttack();		
		},
		Duration,
		false
	);
}
