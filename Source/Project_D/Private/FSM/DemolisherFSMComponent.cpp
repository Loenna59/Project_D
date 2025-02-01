// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/DemolisherFSMComponent.h"
#include "FSM/DemolisherAttackState.h"
#include "FSM/DemolisherWalkState.h"

void UDemolisherFSMComponent::SetupState(ABaseZombie* Zombie)
{
	//Super::SetupState(Zombie);

	ZombieCharacter = Zombie;

	StateMap.Add(EEnemyState::IDLE, NewObject<UIdleZombieState>(this));
	StateMap.Add(EEnemyState::WALK, NewObject<UDemolisherWalkState>(this));
	StateMap.Add(EEnemyState::ATTACK, NewObject<UDemolisherAttackState>(this));
	StateMap.Add(EEnemyState::DEATH, NewObject<UDeathZombieState>(this));
}
