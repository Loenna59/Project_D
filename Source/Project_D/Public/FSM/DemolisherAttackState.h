// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieState.h"
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

private:
	float ThrowDuration = 2.f;
	float SwingDuration = 2.f;
	float ChargeDuration = 3.f;
	
	float LongRangeThreshold = 1000.f;
	float ShortRangeThreshold = 200.f;

	FTimerHandle TimerHandle;
	
	void UpdateAttackPattern(ABaseZombie* Zombie);
};
