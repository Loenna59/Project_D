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
	FORCEINLINE void Initialize(
		const float _Interval,
		const float _ThrowDuration,
		const float _ChargeSpeed,
		const float _ChargeAcceleration
	);
	
	virtual void OnEnter(ABaseZombie* Zombie) override;
	virtual void OnUpdate(ABaseZombie* Zombie) override;
	virtual void OnExit(ABaseZombie* Zombie) override;

protected:
	void UpdateAttackPattern(ABaseZombie* Zombie);
	
private:
	FTimerHandle TimerHandle;
	float Interval;
	
	float ThrowDuration;
	
	float ChargeSpeed;
	float ChargeAcceleration;
};
