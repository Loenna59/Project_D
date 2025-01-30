// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	NONE UMETA(DisplayName = "None"),
	IDLE UMETA(DisplayName = "Idle"),
	WALK UMETA(DisplayName = "Walk"),
	ATTACK UMETA(DisplayName = "Attack"),
	// HIT UMETA(DisplayName = "Hit"),
	CRAWL UMETA(DisplayName = "Crawl"),
	DEATH UMETA(DisplayName = "Death"),
};
