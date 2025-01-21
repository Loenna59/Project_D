// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DemolisherAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UDemolisherAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	bool IsWalking;
	
};
