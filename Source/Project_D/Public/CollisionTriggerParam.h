// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionTriggerParam.generated.h"

UCLASS(Abstract)
class PROJECT_D_API ACollisionTriggerParam : public AActor
{
	GENERATED_BODY()

public:
	FHitResult HitResult;
};
