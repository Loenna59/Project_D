// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathZombieState.h"

#include "BaseZombie.h"
#include "Animation/ZombieAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UDeathZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (!Zombie->GetMesh()->IsSimulatingPhysics())
		{
			if (Zombie->AnimationInstance)
			{
				Zombie->AnimationInstance->bIsDead = true;
			}
		}
		Zombie->OnDead();
	}
}

void UDeathZombieState::OnUpdate(ABaseZombie* Zombie)
{
}

void UDeathZombieState::OnExit(ABaseZombie* Zombie)
{
}
