// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieFSMComponent.h"
#include "DemolisherFSMComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UDemolisherFSMComponent : public UZombieFSMComponent
{
	GENERATED_BODY()

public:
	virtual void SetupState(ABaseZombie* Zombie, const TScriptInterface<IZombieState>& AttackState) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FSM")
	float ThrowDuration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FSM")
	float ChargeSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FSM")
	float ChargeAcceleration = 2.f;
};
