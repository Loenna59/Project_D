// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EPathDirection.h"
#include "PathVector.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UPathVector : public UObject
{
	GENERATED_BODY()
private:
	bool bIsAlternative;
	
public:
	UPathVector() {}

	UPROPERTY()
	class UPathVector* North;
	UPROPERTY()
	class UPathVector* East;
	UPROPERTY()
	class UPathVector* South;
	UPROPERTY()
	class UPathVector* West;

	// 대각선 방향 이웃 노드
	UPROPERTY()
	class UPathVector* NorthEast;
	UPROPERTY()
	class UPathVector* NorthWest;
	UPROPERTY()
	class UPathVector* SouthEast;
	UPROPERTY()
	class UPathVector* SouthWest;

	UPROPERTY()
	class UPathVector* Next;

	FVector Location;
	FVector ExitPoint;
	
	int32 Cost;
	int32 HeuristicCost;
	float Height;
	float SlopeAngle;

	EPathDirection PathDirection = EPathDirection::North;

	void ClearPath();
	void BecomeDestination();
	bool HasPath();
	float GetTotalCost() const;

	bool GetIsAlternative() const
	{
		return bIsAlternative;
	}

	void SetIsAlternative(bool IsAlternative)
	{
		bIsAlternative = IsAlternative;
	}

	class UPathVector* GrowPathTo(UPathVector* Neighbor, EPathDirection Direction, float Weight);
	
	class UPathVector* GrowPathNorth(float Weight);
	class UPathVector* GrowPathEast(float Weight);
	class UPathVector* GrowPathSouth(float Weight);
	class UPathVector* GrowPathWest(float Weight);

	bool CanMoveTo(const UPathVector* Neighbor, float Weight, float Angle) const;

	static void MakeEastWestNeighbors(UPathVector* East, UPathVector* West);
	static void MakeNorthSouthNeighbors(UPathVector* North, UPathVector* South);
	static void MakeNorthWestSouthEastNeighbors(UPathVector* NorthWest, UPathVector* SouthEast);
	static void MakeNorthEastSouthWestNeighbors(UPathVector* NorthEast, UPathVector* SouthWest);
};
