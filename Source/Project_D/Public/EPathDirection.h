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
};

class EPathDirectionExtensions
{
public:
	// 방향에 따른 회전 값 반환
	static FQuat GetRotation(EPathDirection Direction)
	{
		switch (Direction)
		{
			case EPathDirection::North:
				return FQuat(FRotator(0.0f, 90.f, 0.0f));
			case EPathDirection::East:
				return FQuat::Identity;
			case EPathDirection::South:
				return FQuat(FRotator(0.f, -90.f, 0.f));
			case EPathDirection::West:
				return FQuat(FRotator(0.f, 180.f, 0.f));
			default:
				return FQuat::Identity;
		}
	}

	static float GetAngle(EPathDirection Direction)
	{
		switch (Direction)
		{
			case EPathDirection::North: return 90.f;
			case EPathDirection::East: return 0.f;
			case EPathDirection::South: return -90.f;
			case EPathDirection::West: return 180.f;
		}

		return 0.f;
	}

	static FString EnumToString(EPathDirection EnumValue)
	{
		const UEnum* EnumPtr = StaticEnum<EPathDirection>();
		if (!EnumPtr) return FString("Invalid");

		// Enum의 Value를 문자열로 변환
		return EnumPtr->GetNameByValue(static_cast<uint8>(EnumValue)).ToString();
	}
};
