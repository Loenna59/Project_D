// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EPathDirection.generated.h"

UENUM(BlueprintType)
enum class EPathDirection : uint8
{
	North UMETA(DisplayName = "North"),
	East UMETA(DisplayName = "East"),
	South UMETA(DisplayName = "South"),
	West UMETA(DisplayName = "West"),
	NorthEast UMETA(DisplayName = "NorthEast"),
	NorthWest UMETA(DisplayName = "NorthWest"),
	SouthEast UMETA(DisplayName = "SouthEast"),
	SouthWest UMETA(DisplayName = "SouthWest"),
};
