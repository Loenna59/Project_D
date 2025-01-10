// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoneDurability.Add(FName("head"), 15);
	BoneDurability.Add(FName("spine_01"), 20);
	BoneDurability.Add(FName("upperarm_l"), 15);
	BoneDurability.Add(FName("upperarm_r"), 15);
	BoneDurability.Add(FName("lowerarm_l"), 10);
	BoneDurability.Add(FName("lowerarm_r"), 10);
	BoneDurability.Add(FName("hand_l"), 5);
	BoneDurability.Add(FName("hand_r"), 5);
	BoneDurability.Add(FName("thigh_l"), 15);
	BoneDurability.Add(FName("thigh_r"), 15);
	BoneDurability.Add(FName("calf_l"), 10);
	BoneDurability.Add(FName("calf_r"), 10);
	BoneDurability.Add(FName("foot_l"), 5);
	BoneDurability.Add(FName("foot_r"), 5);
}

void ATestCharacter::Gate(bool bOpen)
{
	if (bOpen)
	{
		if (!bIsGateOpen)
		{
			bIsGateOpen = true;
		}
	}
	else
	{
		bIsGateOpen = false;
	}
}


// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementZombie == EMovementState::CLAWING)
	{
		if (!bDoOnce)
		{
			bDoOnce = true;
			GetWorld()->GetTimerManager().SetTimer(
				OnceTimerHandle,
				[this]()
				{
					Gate(true);
				},
				3.f,
				false,
				-1.f // 첫 실행까지의 대기 시간 (-1은 DelayTime과 동일)
			);
		}

		if (bIsGateOpen)
		{
			SetCapsuleLocation();
			AddForceToBones();
		}
		
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimerHandle,
			[this]()
			{
				FHitResult Hit;
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(this);
				
                FVector Start = GetMesh()->GetSocketLocation(HitBoneName);
				FVector End = Start + FVector::DownVector * 1000.f;
				
				bool bHit = GetWorld()->LineTraceSingleByChannel(
					Hit,
					Start,
					End,
					ECC_Visibility,
					CollisionParams
				);

				if (bHit)
				{
					FVector Location = Hit.Location;
                    FRotator Rotation = UKismetMathLibrary::MakeRotFromX(Hit.Normal);
                    FVector DecalSize(-31.5f, -64.f, -64.f);
            
                    //Decal 찍기
                    //UGameplayStatics::SpawnDecalAtLocation()
				}
			},
			1.f,
			false,
			-1.f // 첫 실행까지의 대기 시간 (-1은 DelayTime과 동일)
		);
	}

}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATestCharacter::AnyDamage(int32 Damage, const FName& BoneName, AActor* Causer)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, BoneName.ToString());
	AppliedDamage = Damage;
	HitBoneName = BoneName;
	DamageCauser = Causer;

	RenameBoneName();

	if (ApplyDamageToBone())
	{
		Dismemberment();
		ChangeMovementType();
		ApplyPhysics();
	}
}

void ATestCharacter::RenameBoneName()
{
	if (HitBoneName == FName("pelvis") ||
	HitBoneName == FName("spine_02") ||
	HitBoneName == FName("spine_03"))
	{
		HitBoneName = FName("spine_01");
	}
}

bool ATestCharacter::ApplyDamageToBone()
{
	if (int32* Value = BoneDurability.Find(HitBoneName))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, HitBoneName.ToString());

		BoneDurability[HitBoneName] -= AppliedDamage;

		return BoneDurability[HitBoneName] <= 0;
	}

	return false;
}

void ATestCharacter::Dismemberment()
{
	GetMesh()->BreakConstraint(CalculateImpulse(), GetBoneSocketLocation(), HitBoneName);

	FTransform MeshSocketTransform = GetMesh()->GetSocketTransform(HitBoneName);

	// Effect 붙이기 (피)
	// UGameplayStatics::SpawnEmitterAttached()
	
	HasSpineBroken = HitBoneName == FName("spine_01");
	BrokenBones.Add(HitBoneName);
	
	FVector WorldLocation = GetMesh()->GetComponentLocation(); // (== GetWorldLocation (blueprint))

	FVector End = WorldLocation + FVector::DownVector * 1000.f;

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		WorldLocation,
		End,
		ECC_Visibility,
		CollisionParams
	);

	// DrawDebugLine(
	// 	GetWorld(),
	// 	WorldLocation,
	// 	End,
	// 	FColor::Red,
	// 	false,
	// 	2.f,
	// 	0,
	// 	1.f
	// 	);

	if (bHit)
	{
		FVector Location = Hit.Location;
		FRotator Rotation = UKismetMathLibrary::MakeRotFromX(Hit.Normal);
		FVector DecalSize(-63.f, -128.f, -128.f);

		//Decal 찍기
		//UGameplayStatics::SpawnDecalAtLocation()
	}
}

void ATestCharacter::ChangeMovementType()
{
	if (MovementZombie == EMovementState::NORMAL)
	{
		MovementZombie = EMovementState::CLAWING;
	}
}

void ATestCharacter::ApplyPhysics()
{
	if (HitBoneName == FName("head") ||
	HitBoneName == FName("spine_01") ||
	HitBoneName == FName("thigh_l") ||
	HitBoneName == FName("thigh_r") ||
	HitBoneName == FName("foot_l") ||
	HitBoneName == FName("foot_r") ||
	HitBoneName == FName("calf_l") ||
	HitBoneName == FName("calf_r"))
	{
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->AddImpulseAtLocation(
			CalculateImpulse(),
			GetBoneSocketLocation(),
			HitBoneName
		);

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ATestCharacter::SetCapsuleLocation()
{
	FVector HeadLocation = GetMesh()->GetSocketLocation("head");
	GetCapsuleComponent()->SetWorldLocation(HeadLocation);
}

void ATestCharacter::AddForceToBones()
{
	GetMesh()->AddForce(
		CalculateCrawlForce(500.f, 3000.f),
		"head",
		true
	);

	TArray<FName> Array_R{"upperarm_r", "lowerarm_r", "hand_r"};

	if (FindBrokenBones(Array_R))
	{
		TArray<FName> Array_L{"upperarm_l", "lowerarm_l", "hand_l"};
		if (!FindBrokenBones(Array_L))
		{
			GetMesh()->AddForce(
				CalculateCrawlForce(1000.f, 1000.f),
				"hand_l",
				true
			);
		}
	}
	else
	{
		GetMesh()->AddForce(
			CalculateCrawlForce(1000.f, 1000.f),
			"hand_r",
			true
		);
	}
}

FVector ATestCharacter::CalculateImpulse()
{
	if (GetWorld())
	{
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
		{
			FRotator Rotation = CameraManager->GetCameraRotation();
			FVector ForwardVector = UKismetMathLibrary::GetForwardVector(Rotation);
			double ForwardRandom = UKismetMathLibrary::RandomFloatInRange(80000.f, 120000.f);

			double UpRandom = UKismetMathLibrary::RandomFloatInRange(5000.f, 8000.f);

			return UpRandom * GetActorUpVector() + ForwardRandom * ForwardVector;			
		}
	}

	return FVector::ZeroVector;
}

FVector ATestCharacter::GetBoneSocketLocation()
{
	return GetMesh()->GetSocketLocation(HitBoneName);
}

FVector ATestCharacter::CalculateCrawlForce(float ForwardForce, float UpwardForce)
{
	FVector CapsuleWorldLocation = GetCapsuleComponent()->GetComponentLocation();
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(CapsuleWorldLocation, DamageCauser->GetActorLocation());

	float Force = HasSpineBroken? 1.f : 1.2f;
	float UpForce = UpwardForce * Force;

	return UKismetMathLibrary::GetForwardVector(Rotation) * ForwardForce * Force + FVector::UpVector * UpForce;
}

bool ATestCharacter::FindBrokenBones(TArray<FName> BoneNames)
{
	for (FName BonName : BoneNames)
	{
		if (BrokenBones.Contains(BonName))
		{
			return true;
		}
	}

	return false;
}

void ATestCharacter::RegdollDeath()
{
	GetMesh()->SetSimulatePhysics(false);
	GetCharacterMovement()->DisableMovement();
}






