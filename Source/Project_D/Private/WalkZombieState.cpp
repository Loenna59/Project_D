// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkZombieState.h"

#include "BaseZombie.h"
#include "BiterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWalkZombieState::OnEnter(ABaseZombie* Zombie)
{
	if (Zombie)
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), "Walk On Enter");

		if (!Zombie->bIsSetupPathFinding)
		{
			Zombie->bIsSetupPathFinding = Zombie->MoveNextField(Zombie->GetPlacedPathField());
			Zombie->InitializePathFinding();
		}
		else
		{
			Zombie->FromLocation = Zombie->GetActorLocation();
		}

		Progress = 0.f;

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
			Zombie->PrepareNextPathFinding();
		}

		FVector Lerp = FMath::Lerp(Zombie->FromLocation, Zombie->ToLocation, Progress);
		Zombie->SetActorLocation(Lerp);

		if (Zombie->PathDirectionChange != EPathDirectionChange::None)
		{
			float Angle = FMath::Lerp(Zombie->DirectionAngleFrom, Zombie->DirectionAngleTo, Progress);
			Zombie->SetActorRelativeRotation(FRotator(0, Angle, 0));
		}
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
