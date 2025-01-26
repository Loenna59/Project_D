// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EPathDirection.h"
#include "UObject/Object.h"
#include "EPathDirectionChange.generated.h"

UENUM(BlueprintType)
enum EPathDirectionChange : uint8
{
	None UMETA(DisplayName = "None"),
	TurnRight UMETA(DisplayName = "TurnRight"),
	TurnLeft UMETA(DisplayName = "TurnLeft"),
	TurnAround UMETA(DisplayName = "TurnAround"),
};

class EPathDirectionChangeExtensions
{
public:
	static EPathDirectionChange GetDirectionChangeTo(EPathDirection Current, EPathDirection Next)
	{

		
		if (Current == Next)
		{
			return None;
		}
		else
		if (static_cast<uint8>(Current) - 1 == static_cast<uint8>(Next) ||
			static_cast<uint8>(Current) + 3 == static_cast<uint8>(Next))
		{
			return TurnRight;
		}
		else
		if (static_cast<uint8>(Current) + 1 == static_cast<uint8>(Next) ||
			static_cast<uint8>(Current) - 3 == static_cast<uint8>(Next))
		{
			return TurnLeft;
		}

		return TurnAround;
	}

	static FString EnumToString(EPathDirectionChange EnumValue)
	{
		const UEnum* EnumPtr = StaticEnum<EPathDirectionChange>();
		if (!EnumPtr) return FString("Invalid");

		// Enum의 Value를 문자열로 변환
		return EnumPtr->GetNameByValue(static_cast<uint8>(EnumValue)).ToString();
	}
};