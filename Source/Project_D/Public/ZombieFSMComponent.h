// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EEnemyState.h"
#include "AttackZombieState.h"
#include "ClawingZombieState.h"
#include "IdleZombieState.h"
#include "WalkZombieState.h"
#include "Components/ActorComponent.h"
#include "ZombieFSMComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECT_D_API UZombieFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UZombieFSMComponent();

	void ChangeState(EEnemyState NewState, ABaseZombie* Zombie);

protected:
	bool bSetupCompleted = false;
	
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetupState();

	UPROPERTY()
	EEnemyState CurrentState = EEnemyState::NONE;

	UPROPERTY()
	TMap<EEnemyState, TScriptInterface<IZombieState>> StateMap;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};