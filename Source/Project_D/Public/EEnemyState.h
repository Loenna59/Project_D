// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	NONE,
	IDLE,
	WALK,
	ATTACK,
	CLAWING,
	DEATH
};

FORCEINLINE const FString EnumToString(EEnemyState State)
{
	switch (State)
	{
	case EEnemyState::NONE:
		return "None";
	case EEnemyState::IDLE:
		return "Idle";
	case EEnemyState::WALK:
		return "Walk";
	case EEnemyState::ATTACK:
		return "Attack";
	case EEnemyState::CLAWING:
		return "Clawing";
	case EEnemyState::DEATH:
		return "Death";
	}

	return "";
}
