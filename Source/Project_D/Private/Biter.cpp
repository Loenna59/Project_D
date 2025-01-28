// Fill out your copyright notice in the Description page of Project Settings.


#include "Biter.h"

#include "PlayerCharacter.h"
#include "TraceChannelHelper.h"
#include "Animation/ZombieAnimInstance.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pathfinding/ZombieAIController.h"

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

	ABaseZombie::SetCollisionPartMesh(Head);
	ABaseZombie::SetCollisionPartMesh(LeftArm);
	ABaseZombie::SetCollisionPartMesh(RightArm);
	ABaseZombie::SetCollisionPartMesh(LeftLeg);
	ABaseZombie::SetCollisionPartMesh(RightLeg);

	AttackPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(GetMesh());
	
	AttackPoint->SetBoxExtent(FVector(50, 50, 50));
	AttackPoint->SetCollisionProfileName(TEXT("EnemyAttack"));
	AttackPoint->SetGenerateOverlapEvents(true);
	
	SetActiveAttackCollision(false);
}

void ABiter::BeginPlay()
{
	Super::BeginPlay();

	PartMeshes.Add(EBodyPart::Head, Head);
	PartMeshes.Add(EBodyPart::LeftLeg, LeftLeg);
	PartMeshes.Add(EBodyPart::RightLeg, RightLeg);
	PartMeshes.Add(EBodyPart::LeftArm, LeftArm);
	PartMeshes.Add(EBodyPart::RightArm, RightArm);

	GetCharacterMovement()->MaxWalkSpeed = 100.f;

	if (GetMesh()->DoesSocketExist(TEXT("AttackSocket")))
	{
		AttackPoint->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("AttackSocket"));
	}
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
