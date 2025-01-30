// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM/DemolisherFSMComponent.h"

void UDemolisherFSMComponent::SetupState(ABaseZombie* Zombie)
{
	//Super::SetupState(Zombie);

	ZombieCharacter = Zombie;
}
