// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController.h"
#include "Components/SplineComponent.h"
#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API AZombieAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY()
	TArray<FVector> SplinePoints;

	int32 CurrentSplinePointIndex;
	
public:
	void SetTarget(AActor* Target);

	void SetSplinePoint(const USplineComponent* Spline);

	void MoveToTarget();
	
	void MoveAlongSpline();

	virtual void StopMovement() override;

	bool bIsPaused = false;

	UPROPERTY()
	class AActor* TargetActor;
};
