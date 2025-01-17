// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionTriggerParam.h"
#include "UObject/Interface.h"
#include "CollisionTrigger.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCollisionTrigger : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_D_API ICollisionTrigger
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnTriggerEnter(class AActor* OtherActor, class ACollisionTriggerParam* Param) = 0;
};
