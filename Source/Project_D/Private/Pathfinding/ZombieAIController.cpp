// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinding/ZombieAIController.h"

#include "Components/SplineComponent.h"
#include "Navigation/PathFollowingComponent.h"

void AZombieAIController::BeginPlay()
{
	bSetControlRotationFromPawnOrientation = false;
	Super::BeginPlay();
}

void AZombieAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.IsSuccess())
	{
		// 스플라인 끝에 도달했는지 확인
		if (CurrentSplinePointIndex < SplinePoints.Num())
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(
				this, &AZombieAIController::MoveAlongSpline);
		}
		else
		{
			StopMovement(); // 스플라인 끝에 도달하면 이동 중지
		}
	}
}

void AZombieAIController::SetTarget(AActor* Target)
{
	TargetActor = Target;
	if (TargetActor)
	{
		MoveToTarget();
	}
	else
	{
		StopMovement();
	}
}

void AZombieAIController::SetSplinePoint(const USplineComponent* Spline)
{
	StopMovement();

	for (int32 i = 0, count = Spline->GetNumberOfSplinePoints(); i < count; i++)
	{
		SplinePoints.Add(Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}
}

void AZombieAIController::MoveToTarget()
{
	if (TargetActor)
	{
		MoveToActor(TargetActor, 100.f);
	}
}

void AZombieAIController::MoveAlongSpline()
{
	// Spline의 끝에 도달한 경우 멈추기
	if (CurrentSplinePointIndex >= SplinePoints.Num())
	{
		StopMovement();
		return;
	}
	
	if (SplinePoints.Num() > 0)
	{
		FVector SplinePoint = SplinePoints[CurrentSplinePointIndex];
		MoveToLocation(SplinePoint, 100.f, false);

		// 다음 스플라인 포인트까지의 방향 벡터 계산
		if (CurrentSplinePointIndex < SplinePoints.Num() - 1)
		{
			FVector NextSplinePoint = SplinePoints[CurrentSplinePointIndex + 1];
			FVector Direction = (NextSplinePoint - SplinePoint).GetSafeNormal();
            
			// 방향 벡터를 기반으로 회전값 계산
			FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
            
			// 액터의 방향을 회전값으로 설정
			if (GetOwner())
			{
				GetOwner()->SetActorRotation(NewRotation);
			}
		}
		
		CurrentSplinePointIndex++;
	}
}

void AZombieAIController::StopMovement()
{
	Super::StopMovement();
	
	SplinePoints.Empty();
	CurrentSplinePointIndex = 0;
}
