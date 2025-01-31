// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/ZombieAnimInstance.h"
#include "DemolisherAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UDemolisherAnimInstance : public UZombieAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSprint;

	void SetChargingAttack(bool IsCharging);
};
