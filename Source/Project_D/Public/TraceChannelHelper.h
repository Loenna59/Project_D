// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECT_D_API TraceChannelHelper
{
	
public:
	TraceChannelHelper();
	~TraceChannelHelper();

	static void LineTraceByChannel(
		const UWorld* World,
		AActor* Actor,
		FVector Start,
		FVector End,
		ECollisionChannel CollisionChannel,
		bool IgnoreSelf,
		bool DrawDebug,
		TFunction<void(bool, FHitResult)> Callback
	);

	static void SphereTraceByChannel(
		const UWorld* World,
		AActor* Actor,
		FVector Start,
		FVector End,
		FRotator Rotator,
		ECollisionChannel CollisionChannel,
		float Radius,
		bool IgnoreSelf,
		bool DrawDebug,
		TFunction<void(bool, TArray<struct FHitResult>)> Callback
	);
};