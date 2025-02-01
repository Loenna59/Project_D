// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WalkZombieState.h"
#include "UObject/Object.h"
#include "DemolisherWalkState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UDemolisherWalkState : public UWalkZombieState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(class ABaseZombie* Zombie) override;
	virtual void OnUpdate(class ABaseZombie* Zombie) override;
	virtual void OnExit(class ABaseZombie* Zombie) override;

	float CurrentDistance;
	float CurrentTime;
	
	const float MaxWalkDuration = 3.f;
};
