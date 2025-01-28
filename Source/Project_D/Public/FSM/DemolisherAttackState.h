// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieState.h"
#include "UObject/NoExportTypes.h"
#include "DemolisherAttackState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UDemolisherAttackState : public UObject, public IZombieState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(ABaseZombie* Zombie) override;
	virtual void OnUpdate(ABaseZombie* Zombie) override;
	virtual void OnExit(ABaseZombie* Zombie) override;

	FTimerHandle TimerHandle;
	const float Interval = 1.75f;
};
