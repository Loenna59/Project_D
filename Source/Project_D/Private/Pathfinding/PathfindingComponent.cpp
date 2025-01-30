// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinding/PathfindingComponent.h"

#include "BaseZombie.h"
#include "GameDebug.h"
#include "PathFindingBoard.h"
#include "PathVector.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPathfindingComponent::UPathfindingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPathfindingComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(true);

	if (AActor* TmpActor = UGameplayStatics::GetActorOfClass(GetWorld(), APathFindingBoard::StaticClass()))
	{
		PathFindingBoard = Cast<APathFindingBoard>(TmpActor);
	}

	SplineComponent = NewObject<USplineComponent>(this);
	GetOwner()->AddOwnedComponent(SplineComponent);
	SplineComponent->RegisterComponent();
}


// Called every frame
void UPathfindingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPathfindingComponent::Initialize(AActor* Tracer)
{
	SplineComponent->SetRelativeLocation(Tracer->GetActorLocation());
	CurrentPathIndex = PathFindingBoard->GetFieldIndex(Tracer->GetActorLocation());	
}

void UPathfindingComponent::TraceSpline(const TArray<UPathVector*>& Paths)
{
	if (!SplineComponent || Paths.Num() == 0)
	{
		return;
	}

	SplineComponent->ClearSplinePoints();

	// 경로의 각 포인트를 스플라인에 추가
	for (const UPathVector* Point : Paths)
	{
		SplineComponent->AddSplinePoint(Point->Location, ESplineCoordinateSpace::World);
	}
	
	// 탄젠트 조정
	for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints(); i++)
	{
		FVector ArriveTangent = SplineComponent->GetArriveTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector LeaveTangent = SplineComponent->GetLeaveTangentAtSplinePoint(i, ESplineCoordinateSpace::World);

		// 탄젠트를 직선에 가깝게 설정
		if (i > 0 && i < SplineComponent->GetNumberOfSplinePoints() - 1)
		{
			FVector PrevPoint = SplineComponent->GetLocationAtSplinePoint(i - 1, ESplineCoordinateSpace::World);
			FVector NextPoint = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
			FVector Direction = (NextPoint - PrevPoint).GetSafeNormal();

			ArriveTangent = Direction * 100.0f; // 탄젠트 길이 조정
			LeaveTangent = Direction * 100.0f;
		}

		SplineComponent->SetTangentsAtSplinePoint(i, ArriveTangent, LeaveTangent, ESplineCoordinateSpace::World);
	}
	
	SplineComponent->UpdateSpline();
}

bool UPathfindingComponent::UpdatePath()
{
	if (LastDestIndex != PathFindingBoard->LastDestIndex)
	{
		LastDestIndex = PathFindingBoard->LastDestIndex;
		return true;
	}

	return false;
}

bool UPathfindingComponent::MoveAlongSpline(ABaseZombie* Mover, float Speed, float DeltaTime)
{
	if (!Mover)
	{
		return false;
	}
	if (!SplineComponent || SplineComponent->GetNumberOfSplinePoints() == 0)
	{
		return false;
	}

	float SplineLength = SplineComponent->GetSplineLength();

	// 현재 이동한 거리 업데이트
	Mover->DistanceAlongSpline += Speed * DeltaTime;

	// 스플라인 끝에 도달하면 종료
	if (Mover->DistanceAlongSpline >= SplineLength)
    {
		Mover->DistanceAlongSpline = SplineLength; // 스플라인의 끝으로 고정
		UE_LOG(LogTemp, Warning, TEXT("Zombie reached the end of the path!"));
		return true;
    }
	
	// 스플라인을 따라 좀비의 위치 업데이트
	FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Mover->DistanceAlongSpline, ESplineCoordinateSpace::World);
	FRotator NewRotation = SplineComponent->GetRotationAtDistanceAlongSpline(Mover->DistanceAlongSpline, ESplineCoordinateSpace::World);

	Mover->SetActorLocationAndRotation(NewLocation, NewRotation);

	return false;
}

TArray<class UPathVector*> UPathfindingComponent::GetPaths(ABaseZombie* Mover)
{
	TArray<UPathVector*> Paths;
	if (!PathFindingBoard)
	{
		return Paths;
	}

	if (!Mover)
	{
		return Paths;
	}

	CurrentPathIndex = PathFindingBoard->GetFieldIndex(Mover->GetActorLocation());
	GameDebug::ShowDisplayLog(GetWorld(), FString::FromInt(CurrentPathIndex));
	Paths = PathFindingBoard->FindPaths(CurrentPathIndex);
	
	TraceSpline(Paths);
	
	return Paths;
}
