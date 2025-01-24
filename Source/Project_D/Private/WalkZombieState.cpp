// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "BiterAnimInstance.h"
#include "GameDebug.h"
#include "PathField.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Enter");

		Zombie->bIsSetupPathFinding = Zombie->MoveNextField(Zombie->GetPlacedPathField());

		if (UAnimInstance* const Anim = Zombie->GetMesh()->GetAnimInstance())
		{
			if (UBiterAnimInstance* BiterAnimInstance = Cast<UBiterAnimInstance>(Anim))
			{
				BiterAnimInstance->bIsWalking = Zombie->bIsSetupPathFinding;
			}
		}
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie && Zombie->bIsSetupPathFinding)
	{
		if (UAnimInstance* const Anim = Zombie->GetMesh()->GetAnimInstance())
		{
			if (UBiterAnimInstance* BiterAnimInstance = Cast<UBiterAnimInstance>(Anim))
			{
				BiterAnimInstance->bIsWalking |= true;
			}
		}
		
		Progress += GetWorld()->GetDeltaSeconds() * Zombie->GetCharacterMovement()->MaxWalkSpeed / 100.f;

		if (Progress >= 1.f)
		{
			Zombie->MoveNextField(Zombie->ToPathField);
			Progress -= 1.f;
		}

		FVector Lerp = FMath::Lerp(Zombie->FromLocation, Zombie->ToLocation, Progress);
		Zombie->SetActorLocation(Lerp + FVector::UpVector * 88.0);
		
		FVector Distance = Zombie->ToLocation - Zombie->FromLocation;
		FVector Direction = Distance.GetSafeNormal();
		
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		FRotator SmoothedRotation = UKismetMathLibrary::RLerp(
			Zombie->GetActorRotation(),  // 현재 회전
			LookAtRotation,              // 목표 회전
			GetWorld()->GetDeltaSeconds(), // 보간 속도
			true                          // 짧은 쪽 경로 선택
		);
		
		Zombie->SetActorRotation(SmoothedRotation);
		// Zombie->AddMovementInput(Direction);
	}
	else
	{
		if (UAnimInstance* const Anim = Zombie->GetMesh()->GetAnimInstance())
		{
			if (UBiterAnimInstance* BiterAnimInstance = Cast<UBiterAnimInstance>(Anim))
			{
				BiterAnimInstance->bIsWalking = false;
			}
		}
	}
	
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		Progress = 0.f;
	}
}
