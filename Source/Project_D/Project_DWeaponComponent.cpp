// Copyright Epic Games, Inc. All Rights Reserved.


#include "Project_DWeaponComponent.h"

#include "BaseZombie.h"
#include "BlankTriggerParam.h"
#include "CollisionDebugDrawingPublic.h"
#include "Project_DCharacter.h"
#include "Project_DProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameDebug.h"
#include "KismetTraceUtils.h"
#include "TraceChannelHelper.h"
#include "ZombieTriggerParam.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
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

		if (UCameraComponent* const CameraComponent = Character->GetFirstPersonCameraComponent())
		{
            FVector CameraLocation = CameraComponent->GetComponentLocation();
            FVector CameraForwardVector = CameraComponent->GetForwardVector();

			TraceChannelHelper::LineTraceByChannel(
				World,
				Character,
				SpawnLocation,
				CameraLocation + CameraForwardVector * 10000.f,
				ECC_Visibility,
				true,
				true,
				[this](bool bHit, FHitResult HitResult)
				{
					if (bHit)
					{
						
						AActor* HitActor = HitResult.GetActor();
						
						if (HitResult.Component.IsValid())
						{
							HitActor = HitResult.Component->GetOwner();
							// GameDebug::ShowDisplayLog(GetWorld(), HitActor->GetName());
						}
						
						if (HitActor)
						{
							if (ICollisionTrigger* Trigger = Cast<ICollisionTrigger>(HitActor))
							{
								
								if (ABaseZombie* Zombie = Cast<ABaseZombie>(HitActor))
								{
									AZombieTriggerParam* Param = NewObject<AZombieTriggerParam>();
									Param->Damage = 5;
									Param->HitBoneName = HitResult.BoneName;
									Param->HitResult = HitResult;

									GameDebug::ShowDisplayLog(GetWorld(), HitResult.BoneName.ToString());
									
									Trigger->OnTriggerEnter(HitActor, Param);
								}
								else
								{
									ABlankTriggerParam* Param = NewObject<ABlankTriggerParam>();
									Param->HitResult = HitResult;
									Trigger->OnTriggerEnter(HitActor, Param);
								}
							}
						}
					}
				}
			);
		
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
