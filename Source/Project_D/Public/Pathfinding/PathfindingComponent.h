// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EPathDirection.h"
#include "EPathDirectionChange.h"
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

	void Initialize();
	
	bool bIsSetupPathFinding = false;
	
	EPathDirection PathDirection = EPathDirection::North;
	EPathDirectionChange PathDirectionChange = EPathDirectionChange::None;
	float DirectionAngleFrom;
	float DirectionAngleTo;
	
	UPROPERTY()
	class APathFindingBoard* PathFindingBoard;

	UPROPERTY()
	class UPathVector* FromPathField;
	
	UPROPERTY()
	class UPathVector* ToPathField;

	FVector FromLocation;
	FVector ToLocation;
	
	class UPathVector* GetPlacedPathField() const;
	bool MoveNextField(UPathVector* Start);

	void InitializePathFinding();
	void PrepareNextPathFinding();
	void PrepareForward();
	void PrepareTurnRight();
	void PrepareTurnLeft();
	void PrepareTurnAround();
		
};
