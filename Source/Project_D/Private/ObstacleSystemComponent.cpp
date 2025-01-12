// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleSystemComponent.h"

#include "PlayerHelper.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UObstacleSystemComponent::UObstacleSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UObstacleSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UObstacleSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UObstacleSystemComponent::TriggerOverObstacle() const
{
	bool bDetect;
	FVector HitLocation;
	FRotator ReverseNormal;
	DetectObstacle(bDetect, HitLocation, ReverseNormal, true);
	if (bDetect)
	{
		ScanObstacle(HitLocation, ReverseNormal, true);
	}
}

void UObstacleSystemComponent::DetectObstacle(bool &bOutDetect, FVector &OutHitLocation, FRotator &OutReverseNormal, const bool &bVerbose) const
{
	const auto Owner = GetOwner();
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	FHitResult OutHit;

	for (int i = 0; i < 8; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector ActorLocation = Owner->GetActorLocation();
		const FRotator ActorRotation = Owner->GetActorRotation();
		const FVector TempVector = UPlayerHelper::MoveVectorUpward(
				UPlayerHelper::MoveVectorDownward(ActorLocation, 60.0f),
				i * 20
			);
		const FVector Start = UPlayerHelper::MoveVectorBackward(TempVector, ActorRotation, 30.0f);
		const FVector End = Owner->GetActorForwardVector() * 200.0f + TempVector;

		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			10.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		if (true == bHit)
		{
			break;
		}
	}
	
	bOutDetect = OutHit.bBlockingHit;
	OutHitLocation = OutHit.Location;
	OutReverseNormal = UPlayerHelper::ReverseNormal(OutHit.Normal);
}

void UObstacleSystemComponent::ScanObstacle(const FVector& DetectLocation, const FRotator& ReverseNormal, const bool& bVerbose) const
{
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	FVector Start, End;
	bool bHit;
	
	FHitResult ScanHitResult;
	// 300만큼 위에서 LineTrace를 시작하여 아래쪽(10 단위)으로 차례대로 수행
	for (int i = 0; i < 30; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector TempVector = UPlayerHelper::MoveVectorDownward(UPlayerHelper::MoveVectorUpward(DetectLocation, 300.0f), i * 10);
		Start = UPlayerHelper::MoveVectorBackward(TempVector, ReverseNormal, 20.0f);
		End = UPlayerHelper::MoveVectorForward(TempVector, ReverseNormal, 80.0f);
		bHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			Start,
			End,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerbose ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			ScanHitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		// LineTrace가 벽에 충돌하면
		if (true == bHit)
		{
			break;
		}
	}

	// 벽에 수직 방향인 normal vector를 Z축으로 180도 회전시킨 결과를 저장
	FRotator WallRotation = UPlayerHelper::ReverseNormal(ScanHitResult.Normal);

	// Player 방향에서 가장 가까이 있는 SphereTrace
	FHitResult FirstTopHitResult;

	// Player 방향에서 가장 멀리 있는 SphereTrace
	FHitResult LastTopHitResult;
	
	for (int i = 0; i < 10; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector A = UPlayerHelper::MoveVectorForward(ScanHitResult.Location, WallRotation, i * 20);
		Start = UPlayerHelper::MoveVectorUpward(A, 10.0f);
		End = UPlayerHelper::MoveVectorDownward(Start, 10.0f);
		FHitResult OutHit;
		bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			5.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerbose ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		if (0 == i)
		{
			if (true == bHit)
			{
				FirstTopHitResult = OutHit;
			}
		}
		else
		{
			if (true == bHit)
			{
				LastTopHitResult = OutHit;
			}
			else
			{
				break;
			}
		}
	}

	// 벽의 끝쪽 지점을 구하고 싶다.
	FHitResult EndOfWallHitResult;
	
	Start = UPlayerHelper::MoveVectorForward(LastTopHitResult.ImpactPoint, WallRotation, 20.0f);
	End = LastTopHitResult.ImpactPoint;
	const TArray<AActor*> ActorsToIgnore;
	bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		End,
		10.0f,
		TraceChannel,
		false,
		ActorsToIgnore,
		bVerbose ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		EndOfWallHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	// 뛰어넘을 때(Vaulting) 착지 지점을 구하고 싶다.
	FHitResult VaultLandingHitResult;
	if (true == bHit)
	{
		Start = UPlayerHelper::MoveVectorForward(EndOfWallHitResult.ImpactPoint, WallRotation, 60.0f);
		End = UPlayerHelper::MoveVectorDownward(Start, 180.0f);
		UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			10.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerbose ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			VaultLandingHitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		if (true == bHit)
		{
			
		}
	}
}
