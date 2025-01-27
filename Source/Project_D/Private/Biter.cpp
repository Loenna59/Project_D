// Fill out your copyright notice in the Description page of Project Settings.


#include "Biter.h"

#include "PlayerCharacter.h"
#include "TraceChannelHelper.h"
#include "Animation/ZombieAnimInstance.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pathfinding/ZombieAIController.h"

// void ABiter::SetupInternal()
// {
// 	HeadBone = "Head";
//
// 	RightHandBone = "RightHand";
// 	LeftHandBone = "LeftHand";
//
// 	BoneDurability.Add(HeadBone, 15);
// 	BoneDurability.Add(FName("Spine1"), 20);
// 	BoneDurability.Add(FName("LeftForeArm"), 15);
// 	BoneDurability.Add(FName("LeftArm"), 15);
// 	BoneDurability.Add(FName("RightArm"), 10);
// 	BoneDurability.Add(FName("RightForeArm"), 10);
// 	BoneDurability.Add(RightHandBone, 5);
// 	BoneDurability.Add(LeftHandBone, 5);
// 	BoneDurability.Add(FName("LeftUpLeg"), 15);
// 	BoneDurability.Add(FName("LeftLeg"), 15);
// 	BoneDurability.Add(FName("LeftFoot"), 10);
// 	BoneDurability.Add(FName("RightUpLeg"), 10);
// 	BoneDurability.Add(FName("RightLeg"), 5);
// 	BoneDurability.Add(FName("RightFoot"), 5);
//
// 	BoneArray_R = {"LeftForeArm", "LeftArm", LeftHandBone};
// 	BoneArray_L = {"RightForeArm", "RightArm", RightHandBone};
// }
//
// FName ABiter::RenameBoneName(const FName& HitBoneName)
// {
// 	if (HitBoneName == FName("Spine") || HitBoneName == FName("Spine2"))
// 	{
// 		return FName("Spine1");
// 	}
//
// 	return HitBoneName;
// }
//
// bool ABiter::IsPhysicsBone(const FName& HitBoneName)
// {
// 	return HitBoneName == HeadBone ||
// 		HitBoneName == FName("Spine1") ||
// 		HitBoneName == FName("LeftUpLeg") ||
// 		HitBoneName == FName("LeftLeg") ||
// 		HitBoneName == FName("LeftFoot") ||
// 		HitBoneName == FName("RightUpLeg") ||
// 		HitBoneName == FName("RightLeg") ||
// 		HitBoneName == FName("RightFoot");
// }

ABiter::ABiter()
{
	PrimaryActorTick.bStartWithTickEnabled = true;

	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	LeftArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftArm"));
	RightArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightArm"));
	LeftLeg = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftLeg"));
	RightLeg = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightLeg"));
	
	Head->SetupAttachment(GetMesh());
	LeftArm->SetupAttachment(GetMesh());
	RightArm->SetupAttachment(GetMesh());
	LeftLeg->SetupAttachment(GetMesh());
	RightLeg->SetupAttachment(GetMesh());

	Head->SetLeaderPoseComponent(GetMesh());
	LeftArm->SetLeaderPoseComponent(GetMesh());
	RightArm->SetLeaderPoseComponent(GetMesh());
	LeftLeg->SetLeaderPoseComponent(GetMesh());
	RightLeg->SetLeaderPoseComponent(GetMesh());

	SetCollisionPartMesh(Head);
	SetCollisionPartMesh(LeftArm);
	SetCollisionPartMesh(RightArm);
	SetCollisionPartMesh(LeftLeg);
	SetCollisionPartMesh(RightLeg);

	AttackPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(GetMesh());
	
	if (AttackPoint && GetMesh()->DoesSocketExist(TEXT("AttackSocket")))
    {
    	AttackPoint->SetupAttachment(
    		GetMesh(),
    		TEXT("AttackSocket")
    	);
    }
	
	AttackPoint->SetBoxExtent(FVector(50, 50, 50));
	AttackPoint->SetCollisionProfileName(TEXT("EnemyAttack"));
	AttackPoint->SetGenerateOverlapEvents(true);
	
	SetActiveAttackCollision(false);
}

void ABiter::SetCollisionPartMesh(USkeletalMeshComponent* Part)
{
	Part->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	Part->SetCollisionObjectType(ECC_PhysicsBody);
	Part->SetCollisionResponseToAllChannels(ECR_Block);
	Part->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
}

void ABiter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 100.f;
}

void ABiter::OnTriggerAttack(bool Start)
{
	IsAttacking = Start;
	if (IsAttacking)
	{
		if (AttackTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		}
		
		GetWorld()->GetTimerManager().SetTimer(
			AttackTimerHandle,
			[this]
			{
				TraceChannelHelper::SphereTraceByChannel(
					GetWorld(),
					this,
					AttackPoint->GetComponentLocation(),
					AttackPoint->GetComponentLocation(),
					FRotator::ZeroRotator,
					ECC_Visibility,
					70,
					true,
					true,
					[this] (bool bHit, TArray<FHitResult> HitResults)
					{
						for (FHitResult HitResult : HitResults)
						{
							if (AActor* Actor = HitResult.GetActor())
							{
								if (APlayerCharacter* P = Cast<APlayerCharacter>(Actor))
								{
									// GameDebug::ShowDisplayLog(GetWorld(), "ATTACK");
									P->OnDamaged(10);
								}
							}
						}
					}
				);
				//SetActiveAttackCollision(true);
			},
			AttackTiming,
			false
		);

		AnimationInstance->PlayMontage(AI, AnimState::Attack);
		return;
	}
	// SetActiveAttackCollision(false);
	FSM->ChangeState(EEnemyState::IDLE, this);
}

void ABiter::SetActiveAttackCollision(bool Active) const
{
	AttackPoint->SetVisibility(Active);
	AttackPoint->SetCollisionEnabled(Active? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ABiter::OnOverlappedAttackPoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->OnDamaged(1);
	}
}
