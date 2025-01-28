// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseZombie.h"

#include "ExplosiveCollisionActor.h"
#include "GameDebug.h"
#include "PathFindingBoard.h"
#include "PathVector.h"
#include "PlayerCharacter.h"
#include "TraceChannelHelper.h"
#include "VaultGameModeBase.h"
#include "ZombieTriggerParam.h"
#include "Animation/ZombieAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pathfinding/ZombieAIController.h"
#include "Project_D/Project_DCharacter.h"

// Sets default values
ABaseZombie::ABaseZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);

	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void ABaseZombie::BeginPlay()
{
	Super::BeginPlay();

	CurrentHp = MaxHp;

	GetMesh()->SetMassOverrideInKg(NAME_None, Mass);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	AnimationInstance = Cast<UZombieAnimInstance>(GetMesh()->GetAnimInstance());
	AI = Cast<AZombieAIController>(GetController());

	FSM = NewObject<UZombieFSMComponent>(this);
	AddOwnedComponent(FSM);
	FSM->RegisterComponent();

	FSM->ChangeState(EEnemyState::IDLE, this);

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABaseZombie::OnCollisionHit);

	if (AVaultGameModeBase* VaultGameModeBase = Cast<AVaultGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		VaultGameModeBase->IncreaseCount();
	}
}

void ABaseZombie::SetCollisionPartMesh(USkeletalMeshComponent* Part)
{
	Part->SetCollisionProfileName("Ragdoll");
	// Part->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	// Part->SetCollisionObjectType(ECC_PhysicsBody);
	// Part->SetCollisionResponseToAllChannels(ECR_Block);
	// Part->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	// Part->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
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
						   if (HitActor)
						   {
						   		if (HitActor->IsA<AProject_DCharacter>() || HitActor->IsA<APlayerCharacter>())
						   		{
						   			double Distance = FVector::Distance(HitActor->GetActorLocation(), GetActorLocation());
						   			
									AI->SetTarget(HitActor);
									if (Distance > AttackRadius)
									{
										FSM->ChangeState(EEnemyState::WALK, this);
									}
									else
									{
										// GameDebug::ShowDisplayLog(GetWorld(), FString::SanitizeFloat(Distance));
										FSM->ChangeState(EEnemyState::ATTACK, this);
									}
						   			HitPlayer = true;
						   			break;
						   		}
						   }
					}
				}

				if (!HitPlayer)
				{
					AI->SetTarget(nullptr);
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
		AnimationInstance->PlayMontage(AI, AnimState::Attack);
		return;
	}
	FSM->ChangeState(EEnemyState::IDLE, this);
}

// Called to bind functionality to input
void ABaseZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ABaseZombie::ApplyDamageToBone(EBodyPart Part, int32 Damage)
{
	if (BoneDurability.Contains(Part))
	{
		BoneDurability[Part] -= Damage;
		return BoneDurability[Part] <= 0;
	}
	
	return false;
}

void ABaseZombie::Dismemberment(EBodyPart Part)
{
	BrokenParts.Add(Part);

	if (PartMeshes.Contains(Part))
	{
		USkeletalMeshComponent* PartMesh = PartMeshes[Part];

		PartMesh->SetLeaderPoseComponent(nullptr);
		PartMesh->SetCollisionProfileName("Ragdoll");
		PartMesh->SetSimulatePhysics(true);
		PartMesh->SetCollisionResponseToChannel(ECC_EngineTraceChannel2, ECR_Ignore);

		FDetachmentTransformRules Rules(EDetachmentRule::KeepRelative, true);
		
		PartMesh->DetachFromComponent(Rules);
		
		PartMesh->AddImpulse(CalculateImpulse());
	}
	
	// if (USkeletalMeshComponent* MeshComponent = GetMesh())
	// {
	// 	FTransform SocketTransform = MeshComponent->GetSocketTransform(HitBoneName);
	// 	MeshComponent->BreakConstraint(CalculateImpulse(), SocketTransform.GetLocation(), HitBoneName);
	//
	// 	// 피 효과 부여하려면 여기서
	//
	// 	BrokenParts.Add(HitBoneName);
	//
	// 	if (UWorld* const World = GetWorld())
	// 	{
	// 		FVector Start = MeshComponent->GetComponentLocation(); // WorldLocation
	// 		FVector End = Start + FVector(0, 0, -1000.f);
	// 		
	// 		TraceChannelHelper::LineTraceByChannel(
	// 			World,
	// 			this,
	// 			Start,
	// 			End,
	// 			ECC_Visibility,
	// 			true,
	// 			false,
	// 			[this](bool bHit, FHitResult HitResult)
	// 			{
	// 				FVector Location = HitResult.Location;
	// 				FRotator Rotator = FRotationMatrix::MakeFromX(HitResult.Normal).Rotator();
	// 	
	// 				// 핏자국 찍기 (Decal)
	// 				// FVector DecalScale(-63.f, -128.f, -128.f);
	// 			}
	// 		);
	// 	}
	// }
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

			double RandomForwardInRange = FMath::FRandRange(800.f, 1200.f);
			double RandomUpInRange = FMath::FRandRange(50.f, 80.f);

			return RandomUpInRange * GetActorUpVector() + RandomForwardInRange * ForwardVector;
		}
	}

	return FVector::ZeroVector;
}

bool ABaseZombie::InstantKilled(EBodyPart Part)
{
	return WeaknessParts.Contains(Part);
}

bool ABaseZombie::IsPhysicsBone(EBodyPart Part)
{
	return Part == EBodyPart::Head || Part == EBodyPart::LeftLeg || Part == EBodyPart::RightLeg;
}

bool ABaseZombie::ContainsBrokenBones(TArray<FName> BoneNames)
{
	for (FName BonName : BoneNames)
	{
		EBodyPart Part = BoneRangeMap[BonName];
		if (BrokenParts.Contains(Part))
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

	for (auto Pair : PartMeshes)
	{
		Pair.Value->SetSimulatePhysics(true);
	}

	if (AVaultGameModeBase* VaultGameModeBase = Cast<AVaultGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		VaultGameModeBase->DecreaseCount();
	}

	// FTimerHandle TimerHandle;
	//
	// GetWorld()->GetTimerManager().SetTimer(
	// 	TimerHandle,
	// 	[this] ()
	// 	{
	// 		this->Destroy();
	// 	},
	// 	5.f,
	// 	false
	// );
}

void ABaseZombie::OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param)
{
	if (AZombieTriggerParam* const ZombieParam = Cast<AZombieTriggerParam>(Param))
	{
		FName HitBoneName = ZombieParam->HitBoneName;
		int32 Damage = ZombieParam->Damage;
		
		this->Attacker = OtherActor;

		if (BoneRangeMap.Contains(HitBoneName))
		{
			EBodyPart Part = BoneRangeMap[HitBoneName];
			
			if (ApplyDamageToBone(Part, Damage))
			{
				// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, HitBoneName.ToString());
				// FSM->ChangeState(EEnemyState::CLAWING, this);
				Dismemberment(Part);

				if (InstantKilled(Part))
				{
					FSM->ChangeState(EEnemyState::DEATH, this);
					return;
				}
			}
		}

		CurrentHp -= Damage;
		if (CurrentHp <= 0)
		{
			FSM->ChangeState(EEnemyState::DEATH, this);
			return;
		}

		AnimationInstance->PlayMontage(AI, AnimState::Hit);
	}
}

void ABaseZombie::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA<AExplosiveCollisionActor>())
	{
		GetMesh()->SetSimulatePhysics(true);
		// GameDebug::ShowDisplayLog(GetWorld(), "Death");
		FSM->ChangeState(EEnemyState::DEATH, this);
	}
}

void ABaseZombie::Rotate()
{
	// if (DetectedTarget)
	// {
	// 	FVector Distance = DetectedTarget->GetActorLocation() - GetActorLocation();
	// 	FVector Direction = Distance.GetSafeNormal();
	//
	// 	FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	//
	// 	// 현재 회전과 목표 회전을 선형 보간 (LERP)
	// 	FRotator SmoothedRotation = UKismetMathLibrary::RLerp(
	// 		GetActorRotation(),  // 현재 회전
	// 		LookAtRotation,              // 목표 회전
	// 		GetWorld()->GetDeltaSeconds(), // 보간 속도
	// 		true                          // 짧은 쪽 경로 선택
	// 	);
	// 	
	// 	SetActorRelativeRotation(SmoothedRotation);
	// 	// Pathfinding->DirectionAngleFrom = LookAtRotation.Yaw;
	// }
}
