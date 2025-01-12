// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerHelper.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UPlayerHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static FVector MoveVectorUpward(const FVector& InVector, const float AddValue);
	UFUNCTION(BlueprintPure)
	static FVector MoveVectorDownward(const FVector& InVector, const float SubValue);
	UFUNCTION(BlueprintPure)
	static FVector MoveVectorForward(const FVector& InVector, const FRotator& InRotation, const float AddValue);
	UFUNCTION(BlueprintPure)
	static FVector MoveVectorBackward(const FVector& InVector, const FRotator& InRotation, const float SubValue);
	UFUNCTION(BlueprintPure)
	static FVector MoveVectorLeft(const FVector& InVector, const FRotator& InRotation, const float MoveValue);
	UFUNCTION(BlueprintPure)
	static FVector MoveVectorRight(const FVector& InVector, const FRotator& InRotation, const float MoveValue);
	UFUNCTION(BlueprintPure)
	static FRotator ReverseNormal(const FVector& InNormal);
};
