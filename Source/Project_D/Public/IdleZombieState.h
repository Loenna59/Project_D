// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieState.h"
#include "IdleZombieState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UIdleZombieState : public UObject, public IZombieState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(class ABaseZombie* Zombie) override;
	virtual void OnUpdate(class ABaseZombie* Zombie) override;
	virtual void OnExit(class ABaseZombie* Zombie) override;
};
