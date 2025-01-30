// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "GameDebug.h"
#include "PathFindingBoard.h"
#include "Animation/ZombieAnimInstance.h"
#include "Pathfinding/PathfindingComponent.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->AnimationInstance)
		{
			Zombie->AnimationInstance->bIsWalking = true;
		}

		// if (AZombieAIController* AI = Cast<AZombieAIController>(Zombie->GetController()))
		// {
		// 	AI->MoveToTarget();
		// }
		
		Path = Zombie->Pathfinding->GetPaths();
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->Pathfinding->UpdatePath())
		{
			Path = Zombie->Pathfinding->GetPaths();
		}
		
		if (Path.Num() > 0)
		{
			float DeltaTime = GetWorld()->DeltaTimeSeconds;
			FVector TargetLocation = Path[0]->Location;
			FVector TargetDirection = (TargetLocation - Zombie->GetActorLocation()).GetSafeNormal();
			CurrentVelocity = FMath::VInterpTo(CurrentVelocity, TargetDirection * MovementSpeed, DeltaTime, 2.f);
			
			// 회전 보간
			FRotator TargetRotation = TargetDirection.Rotation();
			Zombie->SetActorRotation(FMath::RInterpTo(Zombie->GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
	
			// 이동 적용
			Zombie->SetActorLocation(Zombie->GetActorLocation() + CurrentVelocity * DeltaTime);
	
			// 목표 지점에 도달했는지 확인 (거리 체크)
			float DistanceToTarget = FVector::Dist(Zombie->GetActorLocation(), TargetLocation);
			if (DistanceToTarget < AcceptanceRadius)
			{
				// 경로의 첫 번째 노드 제거
				Path.RemoveAt(0);
	
				// 경로가 끝나면 멈춤
				if (Path.Num() == 0)
				{
					CurrentVelocity = FVector::ZeroVector;
				}
			}
			else
			{
				GameDebug::ShowDisplayLog(GetWorld(), FString::Printf(TEXT("남은 path %d %f"), Path.Num(), DistanceToTarget));
			}
		}
	}
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		if (Zombie->AnimationInstance)
		{
			Zombie->AnimationInstance->bIsWalking = false;
		}

		Path.Empty();
	}
}
