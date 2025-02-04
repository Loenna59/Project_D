// Fill out your copyright notice in the Description page of Project Settings.


#include "GasTank.h"

#include "ExplosiveCollisionActor.h"
#include "PlayerCharacter.h"
#include "TraceChannelHelper.h"
#include "VaultGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceActor.h"


AGasTank::AGasTank()
{
	PrimaryActorTick.bCanEverTick = true;

	LeftArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftArm"));
	RightArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightArm"));
	LeftLeg = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftLeg"));
	RightLeg = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightLeg"));
	
	LeftArm->SetupAttachment(GetMesh());
	RightArm->SetupAttachment(GetMesh());
	LeftLeg->SetupAttachment(GetMesh());
	RightLeg->SetupAttachment(GetMesh());

	LeftArm->SetLeaderPoseComponent(GetMesh());
	RightArm->SetLeaderPoseComponent(GetMesh());
	LeftLeg->SetLeaderPoseComponent(GetMesh());
	RightLeg->SetLeaderPoseComponent(GetMesh());

	AttackPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(GetMesh());
	
	AttackPoint->SetCollisionProfileName(TEXT("EnemyAttack"));
	AttackPoint->SetGenerateOverlapEvents(true);

	GasCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GasCylinder"));
	GasCylinder->SetupAttachment(GetMesh());
	
	GasCylinder->SetCollisionProfileName("Enemy");
	GasCylinder->SetGenerateOverlapEvents(true);
}

void AGasTank::BeginPlay()
{
	Super::BeginPlay();

	PartMeshes.Add(EBodyPart::LeftLeg, LeftLeg);
	PartMeshes.Add(EBodyPart::RightLeg, RightLeg);
	PartMeshes.Add(EBodyPart::LeftArm, LeftArm);
	PartMeshes.Add(EBodyPart::RightArm, RightArm);

	SetCollisionPartMesh(LeftArm);
	SetCollisionPartMesh(RightArm);
	SetCollisionPartMesh(LeftLeg);
	SetCollisionPartMesh(RightLeg);

	JetBalloonComponent = NewObject<UJetBalloonComponent>(this);
	AddOwnedComponent(JetBalloonComponent);
	JetBalloonComponent->RegisterComponent();
	
	if (GetMesh()->DoesSocketExist(TEXT("AttackSocket")))
	{
		AttackPoint->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("AttackSocket"));
	}

	if (GetMesh()->DoesSocketExist(TEXT("CylinderSocket")))
	{
		GasCylinder->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("CylinderSocket"));
	}
}

void AGasTank::Tick(float DeltaSeconds)
{
	if (bIsExplosion)
	{
		JetBalloonComponent->StartSimulate(GetMesh());
		DeadTime += DeltaSeconds;

		if (DeadTime >= DelayDeadTime)
		{
			FTransform SpawnTransform(FRotator::ZeroRotator, GetMesh()->GetComponentLocation(), FVector::OneVector);

			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ExplosionVFX,
				GetMesh()->GetComponentLocation(),
				FRotator::ZeroRotator,
				FVector::OneVector * 4.f
			);

			GetWorld()->SpawnActor<AExplosiveCollisionActor>(AExplosiveCollisionActor::StaticClass(), SpawnTransform);
			GetWorld()->SpawnActor<ARadialForceActor>(ARadialForceActor::StaticClass(), SpawnTransform);

			FSM->ChangeState(EEnemyState::DEATH, this);

			this->Destroy();
		}
	}
	else
	{
		Super::Tick(DeltaSeconds);
	}

}

void AGasTank::OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param)
{
	if (GasTankDurablity > 0 && Param->HitResult.Component == GasCylinder)
	{
		GasTankDurablity -= 1;
		if (GasTankDurablity <= 0)
		{
			bIsExplosion = true;
			Evaluate();
		}
	}
	else
	{
		Super::OnTriggerEnter(OtherActor, Param);
	}
}

void AGasTank::OnDead()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeadSFX, GetActorLocation(), FRotator::ZeroRotator, 0.5f);

	Super::OnDead();
}
