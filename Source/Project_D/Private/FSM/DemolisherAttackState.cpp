// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/DemolisherAttackState.h"

#include "BaseZombie.h"
#include "Demolisher.h"
#include "GameDebug.h"

void UDemolisherAttackState::OnEnter(ABaseZombie* Zombie)
{
	if (!Zombie || !GetWorld())
	{
		return;
	}

	// 기존 타이머 초기화
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}

	if (Zombie && Zombie->CurrentHp > 0) // 좀비가 살아있는지 확인
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
	if (!Zombie || !GetWorld())
	{
		return;
	}

	Zombie->OnTriggerAttack(true);
	ADemolisher* Demolisher = Cast<ADemolisher>(Zombie);

	if (!Demolisher)
	{
		Zombie->FinishAttack();
		return;
	}

	// 공격 타입에 따라 지속 시간 설정
	float Duration = 0.0f;
	float DistanceToPlayer = Zombie->CalculateDistanceToTarget();
	
	if (DistanceToPlayer < 0)
	{
		// 에러 처리 (플레이어를 찾을 수 없음)
		Zombie->FinishAttack();
		return;
	}

	GameDebug::ShowDisplayLog(GetWorld(), FString::FromInt(DistanceToPlayer));

	// 거리에 따라 공격 타입 선택
	if (DistanceToPlayer >= LongRangeThreshold)
	{
		// 돌 던지기
		Duration = ThrowDuration;
		Demolisher->Throw();
	}
	else if (DistanceToPlayer < ShortRangeThreshold)
	{
		Duration = SwingDuration;
		Demolisher->Swing();
	}
	else
	{
		Duration = ChargingDuration;
		Demolisher->ChargeTo();
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
