// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Player = Cast<APlayerCharacter>(GetOwningActor());
	if (Player)
	{
		Movement = Player->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		// Set velocity and ground speed from the movement components velocity. Ground speed is calculated from only the X and Y axis of the velocity, so moving up or down does not affect it.
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();

		// Set Should Move to true only if ground speed is above a small threshold (to prevent incredibly small velocities from triggering animations) and if there is currently acceleration (input) applied.
		bShouldMove = (GroundSpeed > 3.0f) && (UKismetMathLibrary::NotEqual_VectorVector(Movement->GetCurrentAcceleration(), FVector::ZeroVector, 0.0f));

		// Set Is Falling from the movement components falling state.
		bIsFalling = Movement->IsFalling();

		bIsDead = Player->bIsDead;
		
		PlayerState = Player->State;
		MovementVector = Player->MovementVector;
	}
}