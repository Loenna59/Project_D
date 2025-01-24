// Fill out your copyright notice in the Description page of Project Settings.

#include "PathVector.h"

#include "PathFindingBoard.h"
#include "TraceChannelHelper.h"

FQuat UPathVector::NorthRotation = FQuat(FRotator(0.f, 180.f, 0.f));
FQuat UPathVector::EastRotation = FQuat(FRotator(0.f, 90.f, 0.f));
FQuat UPathVector::SouthRotation = FQuat(FRotator(0.f, 0.f, 0.f));
FQuat UPathVector::WestRotation = FQuat(FRotator(0.f, 270.f, 0.f));

void UPathVector::ClearPath()
{
	Cost = TNumericLimits<int32>::Max();
	Next = nullptr;
}

void UPathVector::BecomeDestination()
{
	Cost = 0;
	Next = nullptr;
}

bool UPathVector::HasPath()
{
	return Cost != TNumericLimits<int32>::Max();
}

void UPathVector::ShowPath()
{
}

class UPathVector* UPathVector::GrowPathTo(UPathVector* Neighbor, EPathDirection Direction, float Weight)
{
	if (!HasPath() || !Neighbor || Neighbor->HasPath())
	{
		return nullptr;
	}

	float HeightDiff = Neighbor->Height - Height; //+: 오르막 -: 내리막
	float HeightCost = FMath::Abs(HeightDiff);

	if (HeightCost < Weight)
	{
		HeightCost = 0;
	}

	//float SlopeFactor = HeightDiff > 0? 2.f : 1.f;

	//λ = lamda = 2

	float CalculateDistance = this->Cost + 1 + 2 * HeightCost;

	Neighbor->Cost = CalculateDistance;
	Neighbor->Next = this;
	Neighbor->PathDirection = Direction;
	
	return Neighbor;
}

class UPathVector* UPathVector::GrowPathNorth(float Weight)
{
	return GrowPathTo(North, EPathDirection::North, Weight);
}

class UPathVector* UPathVector::GrowPathEast(float Weight)
{
	return GrowPathTo(East, EPathDirection::East, Weight);
}

class UPathVector* UPathVector::GrowPathSouth(float Weight)
{
	return GrowPathTo(South, EPathDirection::South, Weight);
}

class UPathVector* UPathVector::GrowPathWest(float Weight)
{
	return GrowPathTo(West, EPathDirection::West, Weight);
}

bool UPathVector::CanMoveTo(UPathVector* Neighbor, float Weight, float Angle) const
{
	if (!Neighbor)
	{
		return false;
	}

	float HeightDiff = FMath::Abs(Neighbor->Height - Height);

	bool bCan = FMath::Abs(SlopeAngle) <= Angle && HeightDiff < Weight;

	return bCan;
}

void UPathVector::MakeEastWestNeighbors(UPathVector* East, UPathVector* West)
{
	if (West)
	{
		West->East = East;
	}

	if (East)
	{
		East->West = West;
	}
}

void UPathVector::MakeNorthSouthNeighbors(UPathVector* North, UPathVector* South)
{
	if (North)
	{
		North->South = South;
	}

	if (South)
	{
		South->North = North;
	}
}
