// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieState.h"
#include "UObject/Object.h"
#include "WalkZombieState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UWalkZombieState : public UObject, public IZombieState
{
	GENERATED_BODY()

public:
	virtual void OnEnter(ABaseZombie* Zombie) override;
	virtual void OnUpdate(ABaseZombie* Zombie) override;
	virtual void OnExit(ABaseZombie* Zombie) override;

	UPROPERTY()
	TArray<class UPathVector*> Path;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MovementSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RotationSpeed = 10.f;
	float AcceptanceRadius = 100.f;
};
