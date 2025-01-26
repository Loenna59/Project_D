// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinding/PathfindingComponent.h"

#include "GameDebug.h"
#include "PathFindingBoard.h"
#include "PathVector.h"
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
}


// Called every frame
void UPathfindingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPathfindingComponent::Initialize()
{
	if (AActor* TmpActor = UGameplayStatics::GetActorOfClass(GetWorld(), APathFindingBoard::StaticClass()))
	{
		PathFindingBoard = Cast<APathFindingBoard>(TmpActor);
		bIsSetupPathFinding = MoveNextField(GetPlacedPathField());
		if (bIsSetupPathFinding)
		{
			InitializePathFinding();
		}
	}
}

bool UPathfindingComponent::MoveNextField(UPathVector* Start)
{
	if (!Start || !Start->Next)
	{
		return false;
	}
	
	FromPathField = Start;
	ToPathField = Start->Next;

	// auto Str = FString::Printf(TEXT("%s %s"), *GetName(), *(FromLocation.ToString()));
	// GameDebug::ShowDisplayLog(GetWorld(), *Str, FColor::Yellow, true);
	//
	// auto Str2 = FString::Printf(TEXT("%s %s"), *GetName(), *(ToLocation.ToString()));
	// GameDebug::ShowDisplayLog(GetWorld(), *Str2, FColor::Red, true);
	
	return true;
}

void UPathfindingComponent::InitializePathFinding()
{
	if (AActor* const Owner = GetOwner())
	{
		FromLocation = Owner->GetActorLocation();
		ToLocation = FromPathField->ExitPoint;
		PathDirection = FromPathField->PathDirection;
		PathDirectionChange = EPathDirectionChange::None;
		DirectionAngleFrom = EPathDirectionExtensions::GetAngle(PathDirection);
		DirectionAngleTo = EPathDirectionExtensions::GetAngle(PathDirection);
	
		FQuat Quat = EPathDirectionExtensions::GetRotation(PathDirection);
		Owner->SetActorRelativeRotation(Quat);
	}
}

void UPathfindingComponent::PrepareNextPathFinding()
{
	FromLocation = ToLocation;
	ToLocation = FromPathField->ExitPoint;
	PathDirectionChange = EPathDirectionChangeExtensions::GetDirectionChangeTo(PathDirection, FromPathField->PathDirection);
	PathDirection = FromPathField->PathDirection;
	DirectionAngleFrom = DirectionAngleTo;

	
	// GameDebug::ShowDisplayLog(GetWorld(), EPathDirectionExtensions::EnumToString(PathDirection));
	GameDebug::ShowDisplayLog(GetWorld(), EPathDirectionChangeExtensions::EnumToString(PathDirectionChange));

	switch (PathDirectionChange)
	{
		case EPathDirectionChange::None:
			PrepareForward();
			break;
		case EPathDirectionChange::TurnRight:
			PrepareTurnRight();
			break;
		case EPathDirectionChange::TurnLeft:
			PrepareTurnLeft();
			break;
		default:
			PrepareTurnAround();
			break;
	}
}

void UPathfindingComponent::PrepareForward()
{
	if (AActor* const Owner = GetOwner())
	{
		FQuat Quat = EPathDirectionExtensions::GetRotation(PathDirection);
		Owner->SetActorRelativeRotation(Quat);
		DirectionAngleTo = EPathDirectionExtensions::GetAngle(PathDirection);
	}
}

void UPathfindingComponent::PrepareTurnRight()
{
	DirectionAngleTo = DirectionAngleFrom + 90;
}

void UPathfindingComponent::PrepareTurnLeft()
{
	DirectionAngleTo = DirectionAngleFrom - 90;
}

void UPathfindingComponent::PrepareTurnAround()
{
	DirectionAngleTo = DirectionAngleFrom + 180;
}

class UPathVector* UPathfindingComponent::GetPlacedPathField() const
{
	if (AActor* const Owner = GetOwner())
	{
		return PathFindingBoard->FindField(Owner->GetActorLocation());
	}

	return nullptr;
}

