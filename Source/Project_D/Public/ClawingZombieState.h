// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieState.h"
#include "UObject/Object.h"
#include "Components/CapsuleComponent.h"
#include "ClawingZombieState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UClawingZombieState : public UObject, public IZombieState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(ABaseZombie* Zombie) override;
	virtual void OnUpdate(ABaseZombie* Zombie) override;
	virtual void OnExit(ABaseZombie* Zombie) override;

protected:
	UPROPERTY()
	FTimerHandle OnceTimerHandle;
	
	void AddForceToBones(ABaseZombie* Zombie);
	FVector CalculateCrawlForce(UCapsuleComponent* Collision, AActor* Attacker, float ForwardForce, float UpwardForce);
};
