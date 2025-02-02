// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/DemolisherFSMComponent.h"
#include "FSM/DemolisherAttackState.h"
#include "FSM/DemolisherWalkState.h"

void UDemolisherFSMComponent::SetupState(ABaseZombie* Zombie)
{
	ZombieCharacter = Zombie;

	UDemolisherAttackState* AttackState = NewObject<UDemolisherAttackState>(this);
	AttackState->Initialize(AttackInterval, ThrowDuration, ChargeSpeed, ChargeAcceleration);

	StateMap.Add(EEnemyState::IDLE, NewObject<UIdleZombieState>(this));
	StateMap.Add(EEnemyState::WALK, NewObject<UDemolisherWalkState>(this));
	StateMap.Add(EEnemyState::ATTACK, AttackState);
	StateMap.Add(EEnemyState::DEATH, NewObject<UDeathZombieState>(this));
}
