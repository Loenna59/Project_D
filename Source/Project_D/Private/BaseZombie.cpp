// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseZombie.h"

#include "ExplosiveCollisionActor.h"
#include "GameDebug.h"
#include "KismetTraceUtils.h"
#include "TraceChannelHelper.h"
#include "ZombieTriggerParam.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Project_D/Project_DCharacter.h"

// Sets default values
ABaseZombie::ABaseZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->Mass = 75.f;
	GetCharacterMovement()->MaxWalkSpeed = 30.f;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
}

void ABaseZombie::SetupInternal()
{
	HeadBone = "head";

	RightHandBone = "hand_r";
	LeftHandBone = "hand_l";

	BoneDurability.Add(FName("head"), 15);
	// BoneDurability.Add(FName("spine_01"), 20);
	BoneDurability.Add(FName("upperarm_l"), 15);
	BoneDurability.Add(FName("upperarm_r"), 15);
	BoneDurability.Add(FName("lowerarm_l"), 10);
	BoneDurability.Add(FName("lowerarm_r"), 10);
	BoneDurability.Add(FName("hand_l"), 5);
	BoneDurability.Add(FName("hand_r"), 5);
	// BoneDurability.Add(FName("thigh_l"), 15);
	// BoneDurability.Add(FName("thigh_r"), 15);
	// BoneDurability.Add(FName("calf_l"), 10);
	// BoneDurability.Add(FName("calf_r"), 10);
	// BoneDurability.Add(FName("foot_l"), 5);
	// BoneDurability.Add(FName("foot_r"), 5);

	BoneArray_R = {"upperarm_r", "lowerarm_r", "hand_r"};
	BoneArray_L = {"upperarm_l", "lowerarm_l", "hand_l"};

	WeaknessBones = { "head" };

	CurrentHp = MaxHp;
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

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABaseZombie::OnCollisionHit);
}

// Called every frame
void ABaseZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (UWorld* const World = GetWorld())
	{
		if (IsAttacking)
		{
			return;
		}
		
		TraceChannelHelper::SphereTraceByChannel(
			World,
			this,
			GetActorLocation(),
			GetActorLocation(),
			FRotator::ZeroRotator,
			ECC_EngineTraceChannel2, // "Player"
			DetectRadius,
			true,
			false,
			[this] (bool bHit, TArray<FHitResult> HitResults)
			{
				bool HitPlayer = false;
				if (bHit)
				{
					for (FHitResult Hit : HitResults)
					{
						   AActor* HitActor = Hit.GetActor();
						   if (HitActor && HitActor->IsA<AProject_DCharacter>())
						   {
							   DetectedTarget = HitActor;
							   HitPlayer = true;
							   break;
						   }
					}
				}

				if (HitPlayer)
				{
					double Distance = FVector::Distance(DetectedTarget->GetActorLocation(), GetActorLocation());
					if (Distance > AttackRadius)
					{
						FSM->ChangeState(EEnemyState::WALK, this);
					}
					else
					{
						// GameDebug::ShowDisplayLog(GetWorld(), FString::SanitizeFloat(Distance));
						FSM->ChangeState(EEnemyState::ATTACK, this);
					}
				}
				else
				{
					FSM->ChangeState(EEnemyState::IDLE, this);
				}
			}
		);
	}
}

void ABaseZombie::OnTriggerAttack(bool Start)
{
	IsAttacking = Start;
	if (IsAttacking)
	{
		if (MontageMap.Contains("Attack"))
		{
			PlayAnimMontage(MontageMap["Attack"], 1.f, "Attack");
		}
		return;
	}
	FSM->ChangeState(EEnemyState::IDLE, this);
}

// Called to bind functionality to input
void ABaseZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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

		if (UWorld* const World = GetWorld())
		{
			FVector Start = MeshComponent->GetComponentLocation(); // WorldLocation
			FVector End = Start + FVector(0, 0, -1000.f);
			
			TraceChannelHelper::LineTraceByChannel(
				World,
				this,
				Start,
				End,
				ECC_Visibility,
				true,
				false,
				[this](bool bHit, FHitResult HitResult)
				{
					FVector Location = HitResult.Location;
					FRotator Rotator = FRotationMatrix::MakeFromX(HitResult.Normal).Rotator();
		
					// 핏자국 찍기 (Decal)
					// FVector DecalScale(-63.f, -128.f, -128.f);
				}
			);
		}
	}
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

bool ABaseZombie::InstantKilled(const FName& HitBoneName)
{
	for (FName BoneName : WeaknessBones)
	{
		if (BoneName.IsEqual(HitBoneName))
		{
			return true;
		}
	}

	return false;
}

bool ABaseZombie::IsPhysicsBone(const FName& HitBoneName)
{
	return HitBoneName == FName("head") ||
		HitBoneName == FName("spine_01"); //||
		// HitBoneName == FName("thigh_l") ||
		// HitBoneName == FName("thigh_r") ||
		// HitBoneName == FName("foot_l") ||
		// HitBoneName == FName("foot_r") ||
		// HitBoneName == FName("calf_l") ||
		// HitBoneName == FName("calf_r");
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

void ABaseZombie::OnDead()
{
	if (USkeletalMeshComponent* const MeshComponent = GetMesh())
	{
		MeshComponent->SetSimulatePhysics(true);
		// MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this] ()
		{
			this->Destroy();
		},
		5.f,
		false
	);
}

void ABaseZombie::OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param)
{
	if (AZombieTriggerParam* const ZombieParam = Cast<AZombieTriggerParam>(Param))
	{
		FName HitBoneName = ZombieParam->HitBoneName;
		int32 Damage = ZombieParam->Damage;
		
		this->Attacker = OtherActor;
	
		FName BoneName = RenameBoneName(HitBoneName);

		// GameDebug::ShowDisplayLog(GetWorld(), HitBoneName.ToString());

		if (ApplyDamageToBone(BoneName, Damage))
		{
			// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, HitBoneName.ToString());
			// FSM->ChangeState(EEnemyState::CLAWING, this);
			Dismemberment(BoneName);
			ApplyPhysics(BoneName);

			if (InstantKilled(BoneName))
			{
				// GameDebug::ShowDisplayLog(GetWorld(), "Death");
				FSM->ChangeState(EEnemyState::DEATH, this);
				return;
			}
		}

		CurrentHp -= Damage;
		if (CurrentHp <= 0)
		{
			FSM->ChangeState(EEnemyState::DEATH, this);
			return;
		}
		
		if (MontageMap.Contains("Hit"))
		{
			PlayAnimMontage(MontageMap["Hit"], 1.5f, "Hit");
		}

	}
}

void ABaseZombie::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//GameDebug::ShowDisplayLog(GetWorld(), "OnCollisionHit");

	if (OtherActor->IsA<AExplosiveCollisionActor>())
	{
		GetMesh()->SetSimulatePhysics(true);
		GameDebug::ShowDisplayLog(GetWorld(), "Death");
		FSM->ChangeState(EEnemyState::DEATH, this);
	}
}
