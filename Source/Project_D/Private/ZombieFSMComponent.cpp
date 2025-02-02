// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieFSMComponent.h"
#include "BaseZombie.h"
#include "GameDebug.h"

// Sets default values for this component's properties
UZombieFSMComponent::UZombieFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

// Called when the game starts
void UZombieFSMComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetComponentTickEnabled(true);
}

void UZombieFSMComponent::SetupState(ABaseZombie* Zombie, const TScriptInterface<IZombieState>& AttackState)
{
	ZombieCharacter = Zombie;
	
	StateMap.Add(EEnemyState::IDLE, NewObject<UIdleZombieState>(this));
	StateMap.Add(EEnemyState::WALK, NewObject<UWalkZombieState>(this));
	StateMap.Add(EEnemyState::ATTACK, AttackState);
	StateMap.Add(EEnemyState::DEATH, NewObject<UDeathZombieState>(this));
}

void UZombieFSMComponent::ChangeState(EEnemyState NewState, ABaseZombie* Zombie)
{
	if (StateMap.IsEmpty())
	{
		return;
	}
	
	if (CurrentState == EEnemyState::DEATH)
	{
		return;
	}
	
	if (CurrentState != NewState)
	{
		if (StateMap.Contains(CurrentState))
		{
			StateMap[CurrentState]->OnExit(Zombie);
		}
		
		CurrentState = NewState;

		
		if (StateMap.Contains(CurrentState))
		{
            // UKismetSystemLibrary::PrintString(GetWorld(), EnumToString(CurrentState));
			StateMap[CurrentState]->OnEnter(Zombie);
		}
	}
}

void UZombieFSMComponent::EvaluateState(ABaseZombie* Zombie)
{
	if (!Zombie)
	{
		return;
	}

	if (Zombie->CurrentHp <= 0)
	{
		ChangeState(EEnemyState::DEATH, Zombie);
		return;
	}

	ChangeState(EEnemyState::IDLE, Zombie);
}

// Called every frame
void UZombieFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
 
	if (ZombieCharacter && StateMap.Contains((CurrentState)))
	{
		StateMap[CurrentState]->OnUpdate(ZombieCharacter);
	}
}

