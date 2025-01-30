// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "Components/ActorComponent.h"
#include "PathfindingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_D_API UPathfindingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPathfindingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY()
	class APathFindingBoard* PathFindingBoard;

	UPROPERTY(VisibleAnywhere)
	class USplineComponent* SplineComponent;

	int32 CurrentPathIndex;
	int32 LastDestIndex;
	
	void Initialize(AActor* Tracer);

	TArray<class UPathVector*> GetPaths(ABaseZombie* Mover);

	void TraceSpline(const TArray<UPathVector*>& Paths) const;

	bool UpdatePath();

	bool MoveAlongSpline(ABaseZombie* Mover, float Speed, float DeltaTime);
};
