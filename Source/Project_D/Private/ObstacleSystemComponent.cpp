// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleSystemComponent.h"

#include "PlayerHelper.h"
#include "PlayerInterface.h"
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

	Initialize();
}

void UObstacleSystemComponent::Initialize()
{
	PlayerInterface = Cast<IPlayerInterface>(GetOwner());
	if (PlayerInterface)
	{
		PlayerMesh = PlayerInterface->GetMesh();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInterface is nullptr"));
	}
}


// Called every frame
void UObstacleSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UObstacleSystemComponent::TriggerOverObstacle()
{
	bool bDetect;
	FVector HitLocation;
	FRotator ReverseNormal;
	DetectObstacle(bDetect, HitLocation, ReverseNormal, true);
	if (bDetect)
	{
		ScanObstacle(HitLocation, ReverseNormal, true);
		MeasureObstacle(true);
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

void UObstacleSystemComponent::ScanObstacle(const FVector& DetectLocation, const FRotator& ReverseNormal, const bool& bVerbose)
{
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	FVector Start, End;
	bool bHit;
	
	// (300 - i * 10)만큼 위에서 LineTrace를 수행한다.
	// 널널하게 20만큼 뒤에서 시작하여 80만큼 앞에까지 선을 긋는다.
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
			FacedObstacleTopHitResult,
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
	
	ObstacleRotation = UPlayerHelper::ReverseNormal(FacedObstacleTopHitResult.Normal);

	// FacedObstacleTopHitResult.Location을 기준으로 20씩 전진시켜가며 널널하게 위아래로 10만큼씩 범위로 하여 SphereTrace
	for (int i = 0; i < 10; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector A = UPlayerHelper::MoveVectorForward(FacedObstacleTopHitResult.Location, ObstacleRotation, i * 20);
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

	// LastTopHitResult는 장애물의 정확한 끝 지점이라고 볼 수 없다. (20씩 전진시켜가며 대강 측정한 것이기 때문)
	// LastTopHitResult 기준에서 20만큼 앞에 있는 위치를 시작으로 LastTopHitResult 까지 SphereTrace 하면 정확한 장애물의 끝 지점을 찾을 수 있다.
	Start = UPlayerHelper::MoveVectorForward(LastTopHitResult.ImpactPoint, ObstacleRotation, 20.0f);
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
		EndOfObstacleHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	// 장애물의 정확한 끝 지점을 찾았다면 해당 지점부터 60만큼 앞에 있는 지점을 기준으로 시작하여 180만큼 아래에 있는 위치까지 SphereTrace
	if (true == bHit)
	{
		Start = UPlayerHelper::MoveVectorForward(EndOfObstacleHitResult.ImpactPoint, ObstacleRotation, 60.0f);
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

void UObstacleSystemComponent::MeasureObstacle(const bool& bVerbose)
{
	// 만약 앞에 벽이 있는게 확실하다면
	if (FacedObstacleTopHitResult.bBlockingHit && FirstTopHitResult.bBlockingHit)
	{
		// Player 전방에 있는 벽의 높이 = 장애물 꼭대기 위치의 Z좌표 - Player의 발 위치 Z좌표
		ObstacleHeight = FirstTopHitResult.ImpactPoint.Z - PlayerInterface->GetBottomZ();
		
		if (bVerbose)
		{
			UE_LOG(LogTemp, Warning, TEXT("감지된 장애물의 높이 : %f"), ObstacleHeight);
		}
	}
	else
	{
		ObstacleHeight = 0.0f;
	}
}
