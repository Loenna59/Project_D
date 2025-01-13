// Copyright Epic Games, Inc. All Rights Reserved.


#include "Project_DWeaponComponent.h"

#include "BaseZombie.h"
#include "CollisionDebugDrawingPublic.h"
#include "Project_DCharacter.h"
#include "Project_DProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "KismetTraceUtils.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UProject_DWeaponComponent::UProject_DWeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UProject_DWeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// // Try and fire a projectile
	// if (ProjectileClass != nullptr)
	// {
	// 	UWorld* const World = GetWorld();
	// 	if (World != nullptr)
	// 	{
	// 		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	// 		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	// 		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	// 		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	//
	// 		//Set Spawn Collision Handling Override
	// 		FActorSpawnParameters ActorSpawnParams;
	// 		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	//
	// 		// Spawn the projectile at the muzzle
	// 		World->SpawnActor<AProject_DProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	// 	}
	// }

	if (UWorld* const World = GetWorld())
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);
		
		FHitResult HitResult;

		bool bHit = World->SweepSingleByChannel(
			HitResult,
			SpawnLocation,
			SpawnLocation + (FVector::ForwardVector * 1000.f),
			SpawnRotation.Quaternion(),
			ECC_Visibility,
			FCollisionShape::MakeBox(FVector::OneVector * 0.5f),
			Params
		);

		DrawDebugBoxTraceSingle(
			World,
			SpawnLocation,
			SpawnLocation + (FVector::ForwardVector * 1000.f),
			FVector::OneVector * 0.5f,
			SpawnRotation,
			EDrawDebugTrace::ForDuration,
			bHit,
			HitResult,
			FColor::Yellow,
			FColor::Green,
			1.f
		);
		
		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				if (ABaseZombie* Zombie = Cast<ABaseZombie>(HitActor))
				{
					Zombie->AnyDamage(5, HitResult.BoneName, Character);
				}
			}
		}
		
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

bool UProject_DWeaponComponent::AttachWeapon(AProject_DCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UProject_DWeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UProject_DWeaponComponent::Fire);
		}
	}

	return true;
}

void UProject_DWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ensure we have a character owner
	if (Character != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
			}
		}
	}

	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}
