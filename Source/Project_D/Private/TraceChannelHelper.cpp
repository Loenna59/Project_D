// Fill out your copyright notice in the Description page of Project Settings.


#include "TraceChannelHelper.h"

#include "KismetTraceUtils.h"

TraceChannelHelper::TraceChannelHelper()
{
}

TraceChannelHelper::~TraceChannelHelper()
{
}

void TraceChannelHelper::LineTraceByChannel(
	const UWorld* World,
	AActor* Actor,
	FVector Start,
	FVector End,
	ECollisionChannel CollisionChannel,
	bool IgnoreSelf,
	bool DrawDebug,
	TFunction<void(bool, FHitResult)> Callback
)
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	if (IgnoreSelf)
	{
		CollisionParams.AddIgnoredActor(Actor);
	}

	bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		CollisionChannel,
		CollisionParams
	);

	if (DrawDebug)
	{
		DrawDebugLineTraceSingle(
			World,
			Start,
			End,
			EDrawDebugTrace::ForDuration,
			bHit,
			HitResult,
			FColor::Yellow,
			FColor::Green,
			1.f
		);
	}

	Callback(bHit, HitResult);
}

void TraceChannelHelper::SphereTraceByChannel(
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
)
{
	TArray<FHitResult> HitResults;
	
	FCollisionQueryParams Params;
	if (IgnoreSelf)
	{
		Params.AddIgnoredSourceObject(Actor);
	}
	
	bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		Rotator.Quaternion(),
		CollisionChannel, // "Player"
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (DrawDebug)
	{
		DrawDebugSphereTraceMulti(
			World,
			Start,
			End,
			Radius,
			EDrawDebugTrace::ForOneFrame,
			bHit,
			HitResults,
			FColor::Yellow,
			FColor::Green,
			1.f
		);
	}

	Callback(bHit, HitResults);
}