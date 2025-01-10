// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieState.generated.h"

UINTERFACE()
class UZombieState : public UInterface
{
	GENERATED_BODY()
};

class IZombieState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(class ABaseZombie* Zombie) = 0;
	virtual void OnUpdate(class ABaseZombie* Zombie) = 0;
	virtual void OnExit(class ABaseZombie* Zombie) = 0;
};
