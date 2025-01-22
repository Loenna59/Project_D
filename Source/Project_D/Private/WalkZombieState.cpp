// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "BiterAnimInstance.h"
#include "GameDebug.h"
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

		Zombie->MoveNextField(Zombie->GetPlacedPathField());
	}
}

void UWalkZombieState::OnUpdate(ABaseZombie* Zombie)
{
	if (Zombie && Zombie->ToPathField)
	{
		float CurrentDist = FVector::Dist(Zombie->ToLocation, Zombie->GetActorLocation());

		// GameDebug::ShowDisplayLog(GetWorld(), FString::SanitizeFloat(CurrentDist));

		if (CurrentDist <= 110 || CurrentDist >= 200) // 길을 잃은 것 같다면 강제로 다음 패스로
		{
			Zombie->MoveNextField(Zombie->ToPathField);
		}
		
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
		Zombie->AddMovementInput(Direction);
	}
}

void UWalkZombieState::OnExit(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Exit");
	}
}
