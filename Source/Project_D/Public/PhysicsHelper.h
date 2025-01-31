// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PhysicsHelper.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UPhysicsHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Physics")
	static float CalculateDuration(float Distance, float InitialVelocity, float Acceleration);
};
