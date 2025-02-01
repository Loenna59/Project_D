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

protected:
	void UpdateAttackPattern(ABaseZombie* Zombie);
	
private:
	float ThrowDuration = 3.f;
	float SwingDuration = 2.f;
	
	float ChargeSpeed = 250.f;
	float ChargeAcceleration = 2.f;

	FTimerHandle TimerHandle;
	const float Interval = 1.75f;
};
