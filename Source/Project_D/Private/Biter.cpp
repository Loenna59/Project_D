// Fill out your copyright notice in the Description page of Project Settings.


#include "Biter.h"

#include "GameDebug.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"

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

void ABiter::BeginPlay()
{
	Super::BeginPlay();

	AttackPoint->OnComponentBeginOverlap.AddDynamic(this, &ABiter::OnOverlappedAttackPoint);
}

void ABiter::OnTriggerAttack(bool Start)
{
	IsAttacking = Start;
	if (IsAttacking)
	{
		if (MontageMap.Contains("Attack"))
		{
			PlayAnimMontage(MontageMap["Attack"], 1.f, "Attack");
		}
		SetActiveAttackCollision(true);
		return;
	}

	SetActiveAttackCollision(false);
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
	if (APlayerCharacter* const Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->OnDamaged(1);
	}
}
