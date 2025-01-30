// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinding/PathfindingComponent.h"

#include "GameDebug.h"
#include "PathFindingBoard.h"
#include "PathVector.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPathfindingComponent::UPathfindingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPathfindingComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(true);

	if (AActor* TmpActor = UGameplayStatics::GetActorOfClass(GetWorld(), APathFindingBoard::StaticClass()))
	{
		PathFindingBoard = Cast<APathFindingBoard>(TmpActor);
	}

	SplineComponent = NewObject<USplineComponent>(this);
	GetOwner()->AddOwnedComponent(SplineComponent);
	SplineComponent->RegisterComponent();
}


// Called every frame
void UPathfindingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPathfindingComponent::Initialize(AActor* Tracer)
{
	SplineComponent->SetRelativeLocation(Tracer->GetActorLocation());
	CurrentPathIndex = PathFindingBoard->GetFieldIndex(Tracer->GetActorLocation());	
}

void UPathfindingComponent::TraceSpline(const TArray<UPathVector*>& Paths)
{
	if (!SplineComponent)
	{
		return;
	}

	if (Paths.Num() == 0)
	{
		return;
	}

	SplineComponent->ClearSplinePoints();

	// 경로의 각 포인트를 스플라인에 추가
	for (const UPathVector* Point : Paths)
	{
		SplineComponent->AddSplinePoint(Point->Location, ESplineCoordinateSpace::World);
	}

	SplineComponent->UpdateSpline();
}

bool UPathfindingComponent::UpdatePath()
{
	if (LastDestIndex != PathFindingBoard->LastDestIndex)
	{
		LastDestIndex = PathFindingBoard->LastDestIndex;
		return true;
	}

	return false;
}

TArray<class UPathVector*> UPathfindingComponent::GetPaths()
{
	TArray<UPathVector*> Paths;
	if (!PathFindingBoard)
	{
		return Paths;
	}

	Paths = PathFindingBoard->FindPaths(CurrentPathIndex);
	
	TraceSpline(Paths);
	
	return Paths;
}
