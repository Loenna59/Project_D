// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionTriggerParam.h"
#include "GameFramework/Actor.h"
#include "ZombieTriggerParam.generated.h"

UCLASS()
class PROJECT_D_API AZombieTriggerParam : public ACollisionTriggerParam
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AZombieTriggerParam();
	
	int32 Damage;
	FName HitBoneName;
	bool bIsSimulatePhysics = false;
	FVector Impulse;
};
