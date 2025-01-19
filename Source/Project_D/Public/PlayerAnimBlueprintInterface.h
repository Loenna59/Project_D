// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionComponent.h" // EActionState 포함
#include "UObject/Interface.h"
#include "PlayerAnimBlueprintInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UPlayerAnimBlueprintInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_D_API IPlayerAnimBlueprintInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//
	UFUNCTION()
	virtual bool SetPlayerActionState(EActionState PlayerActionState) = 0;

	UFUNCTION()
	virtual bool SetMovementVector(const FVector2D& MovementVector) = 0;
};