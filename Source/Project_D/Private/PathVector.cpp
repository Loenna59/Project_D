// Fill out your copyright notice in the Description page of Project Settings.

#include "PathVector.h"

void UPathVector::ClearPath()
{
	Cost = MAX_int32;
	HeuristicCost = MAX_int32;
	Next = nullptr;
}

void UPathVector::BecomeDestination()
{
	Cost = 0;
	HeuristicCost = 0;
	Next = nullptr;
	ExitPoint = Location;
}

bool UPathVector::HasPath()
{
	return Cost != MAX_int32;
}

float UPathVector::GetTotalCost() const
{
	return Cost + HeuristicCost;
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
	Neighbor->ExitPoint = (Neighbor->Location + Location) * 0.5f;
	Neighbor->PathDirection = Direction;
	
	return Neighbor;
}

class UPathVector* UPathVector::GrowPathNorth(float Weight)
{
	return GrowPathTo(North, EPathDirection::South, Weight);
}

class UPathVector* UPathVector::GrowPathEast(float Weight)
{
	return GrowPathTo(East, EPathDirection::West, Weight);
}

class UPathVector* UPathVector::GrowPathSouth(float Weight)
{
	return GrowPathTo(South, EPathDirection::North, Weight);
}

class UPathVector* UPathVector::GrowPathWest(float Weight)
{
	return GrowPathTo(West, EPathDirection::East, Weight);
}

bool UPathVector::CanMoveTo(const UPathVector* Neighbor, float Weight, float Angle) const
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
	if (South)
	{
		South->North = North;
	}
	
	if (North)
	{
		North->South = South;
	}
}
