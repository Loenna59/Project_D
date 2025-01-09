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
	FVector HitLocation, Normal;
	DetectObstacle(bDetect, HitLocation, Normal);
}

void UObstacleSystemComponent::DetectObstacle(bool &bDetectOut, FVector &HitLocationOut, FVector &NormalOut) const
{
	auto Owner = GetOwner();
	
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
	FHitResult OutHit;
	FLinearColor TraceColor = FLinearColor::Red;
	FLinearColor TraceHitColor = FLinearColor::Green;

	for (int i = 0; i < 8; i++)
	{
		TArray<AActor*> ActorsToIgnore;
		FVector ActorLocation = Owner->GetActorLocation();
		FRotator ActorRotation = Owner->GetActorRotation();
		FVector TempVector = UPlayerHelper::MoveVectorUpward(
			UPlayerHelper::MoveVectorDownward(ActorLocation, 60.0f),
			i * 20
			);
		FVector Start = UPlayerHelper::MoveVectorBackward(TempVector, ActorRotation, 30.0f);
		FVector End = Owner->GetActorForwardVector() * 200.0f + TempVector;
		
		bool bHit = UKismetSystemLibrary::SphereTraceSingle(
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
			TraceColor,
			TraceHitColor,
			5.0f
		);

		if (true == bHit)
		{
			break;
		}
	}

	bDetectOut = OutHit.bBlockingHit;
	HitLocationOut = OutHit.Location;
	NormalOut = OutHit.Normal;
	return;
}
