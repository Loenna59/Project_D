// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "BiterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Enter");

		if (UAnimInstance* const Anim = Zombie->GetMesh()->GetAnimInstance())
		{
			if (UBiterAnimInstance* BiterAnimInstance = Cast<UBiterAnimInstance>(Anim))
			{
				BiterAnimInstance->bIsWalking = true;
			}
		}
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie && Zombie->DetectedTarget)
	{	
		FVector Distance = Zombie->DetectedTarget->GetActorLocation() - Zombie->GetActorLocation();
		FVector Direction = Distance.GetSafeNormal();

		FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

		// 현재 회전과 목표 회전을 선형 보간 (LERP)
		FRotator SmoothedRotation = UKismetMathLibrary::RLerp(
			Zombie->GetActorRotation(),  // 현재 회전
			LookAtRotation,              // 목표 회전
			GetWorld()->GetDeltaSeconds(), // 보간 속도
			true                          // 짧은 쪽 경로 선택
		);
		
		Zombie->SetActorRotation(SmoothedRotation);
		Zombie->AddMovementInput(Direction);
		
		// UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Update");
	}
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Exit");
	}
}
