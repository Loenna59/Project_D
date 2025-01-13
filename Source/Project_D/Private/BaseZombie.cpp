// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseZombie.h"

#include "KismetTraceUtils.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Project_D/Project_DCharacter.h"

// Sets default values
ABaseZombie::ABaseZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BodyMesh");
	BodyMesh->SetupAttachment(GetRootComponent());

	// if (UCapsuleComponent* const Capsule = GetCapsuleComponent())
	// {
	// 	Capsule->SetCollisionProfileName(TEXT("Enemy"));
	// 	if (USkeletalMeshComponent* const MeshComponent = GetMesh())
	// 	{
	// 		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 	}
	// }
	
}

void ABaseZombie::SetupInternal()
{
	HeadBone = "head";

	RightHandBone = "hand_r";
	LeftHandBone = "hand_l";

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

	BoneArray_R = {"upperarm_r", "lowerarm_r", "hand_r"};
	BoneArray_L = {"upperarm_l", "lowerarm_l", "hand_l"};
}

// Called when the game starts or when spawned
void ABaseZombie::BeginPlay()
{
	Super::BeginPlay();

	SetupInternal();

	FSM = NewObject<UZombieFSMComponent>(this);
	AddOwnedComponent(FSM);
	FSM->RegisterComponent();

	FSM->ChangeState(EEnemyState::IDLE, this);
}

// Called every frame
void ABaseZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (UWorld* const World = GetWorld())
	{
		TArray<FHitResult> HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredSourceObject(this);
		
		bool bHit = World->SweepMultiByChannel(
			HitResult,
			GetActorLocation(),
			GetActorLocation(),
			FQuat::Identity,
			ECC_EngineTraceChannel2, // "Player"
			FCollisionShape::MakeSphere(500),
			Params
		);

		DrawDebugSphereTraceMulti(
			World,
			GetActorLocation(),
			GetActorLocation(),
			500,
			EDrawDebugTrace::ForOneFrame,
			bHit,
			HitResult,
			FColor::Yellow,
			FColor::Green,
			1.f
		);

		if (bHit)
		{
			bool HitPlayer = false;
			for (FHitResult Hit : HitResult)
			{
				auto HitActor = Hit.GetActor();
				if (HitActor && HitActor->IsA<AProject_DCharacter>())
				{
					FSM->ChangeState(EEnemyState::WALK, this);
					HitPlayer = true;
					break;
					//UKismetSystemLibrary::PrintString(GetWorld(), HitActor->GetActorNameOrLabel());
				}
			}

			if (!HitPlayer)
			{
				FSM->ChangeState(EEnemyState::IDLE, this);
			}
		}
		else
		{
			FSM->ChangeState(EEnemyState::IDLE, this);
		}
	}
}

// Called to bind functionality to input
void ABaseZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseZombie::AnyDamage(int32 Damage, const FName& HitBoneName, class AActor* DamageCauser)
{
	this->Attacker = DamageCauser;
	
	FName BoneName = RenameBoneName(HitBoneName);

	UKismetSystemLibrary::PrintString(GetWorld(), BoneName.ToString());

	if (ApplyDamageToBone(BoneName, Damage))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, HitBoneName.ToString());
		Dismemberment(BoneName);
		FSM->ChangeState(EEnemyState::CLAWING, this);
		ApplyPhysics(BoneName);
	}
}

FName ABaseZombie::RenameBoneName(const FName& HitBoneName)
{
	if (HitBoneName == FName("pelvis") ||
	HitBoneName == FName("spine_02") ||
	HitBoneName == FName("spine_03"))
	{
		return FName("spine_01"); 
	}

	return HitBoneName;
}

bool ABaseZombie::ApplyDamageToBone(const FName& HitBoneName, int32 Damage)
{
	if (BoneDurability.Find(HitBoneName))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, HitBoneName.ToString());

		BoneDurability[HitBoneName] -= Damage;

		return BoneDurability[HitBoneName] <= 0;
	}

	return false;
}

void ABaseZombie::Dismemberment(const FName& HitBoneName)
{
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		FTransform SocketTransform = MeshComponent->GetSocketTransform(HitBoneName);
		MeshComponent->BreakConstraint(CalculateImpulse(), SocketTransform.GetLocation(), HitBoneName);

		// 피 효과 부여하려면 여기서

		BrokenBones.Add(HitBoneName);

		FVector Start = MeshComponent->GetComponentLocation(); // WorldLocation
		FVector End = Start + FVector(0, 0, -1000.f);
		FHitResult Hit;

		if (LineTraceChannel(Hit, Start, End))
		{
			FVector Location = Hit.Location;
			FRotator Rotator = FRotationMatrix::MakeFromX(Hit.Normal).Rotator();

			// 핏자국 찍기 (Decal)
			// FVector DecalScale(-63.f, -128.f, -128.f);
		}
	}
}

bool ABaseZombie::LineTraceChannel(struct FHitResult& HitResult, FVector Start, FVector End)
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (UWorld* const World = GetWorld())
	{
		return World->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECC_Visibility,
			CollisionParams
		);
	}

	return false;
}

void ABaseZombie::ApplyPhysics(const FName& HitBoneName)
{
	if (USkeletalMeshComponent* const MeshComponent = GetMesh())
	{
		if (IsPhysicsBone(HitBoneName))
		{
			FVector SocketLocation = MeshComponent->GetSocketLocation(HitBoneName);
			MeshComponent->SetSimulatePhysics(true);
			MeshComponent->AddImpulseAtLocation(CalculateImpulse(), SocketLocation, HitBoneName);

			if (UCapsuleComponent* const Collision = GetCapsuleComponent())
			{
				Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

// 충격량 계산
FVector ABaseZombie::CalculateImpulse()
{
	if (UWorld* const World = GetWorld())
	{
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0))
		{
			FRotator Rotator = CameraManager->GetCameraRotation();
			FVector ForwardVector = Rotator.Vector();

			double RandomForwardInRange = FMath::FRandRange(80000.f, 120000.f);
			double RandomUpInRange = FMath::FRandRange(5000.f, 8000.f);

			return RandomUpInRange * GetActorUpVector() + RandomForwardInRange * ForwardVector;
		}
	}

	return FVector::ZeroVector;
}

bool ABaseZombie::IsPhysicsBone(const FName& HitBoneName)
{
	return HitBoneName == FName("head") ||
		HitBoneName == FName("spine_01") ||
		HitBoneName == FName("thigh_l") ||
		HitBoneName == FName("thigh_r") ||
		HitBoneName == FName("foot_l") ||
		HitBoneName == FName("foot_r") ||
		HitBoneName == FName("calf_l") ||
		HitBoneName == FName("calf_r");
}

bool ABaseZombie::ContainsBrokenBones(TArray<FName> BoneNames)
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

