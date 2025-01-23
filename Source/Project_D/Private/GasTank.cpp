// Fill out your copyright notice in the Description page of Project Settings.


#include "GasTank.h"

#include "ExplosiveCollisionActor.h"
#include "GameDebug.h"
#include "PlayerCharacter.h"
#include "TraceChannelHelper.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceActor.h"


AGasTank::AGasTank()
{
	PrimaryActorTick.bCanEverTick = true;

	GasCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GasCylinder"));
	GasCylinder->SetupAttachment(GetMesh());
	GasCylinder->SetCollisionProfileName("Enemy");

	AttackPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(GetMesh());
	
	if (AttackPoint && GetMesh()->DoesSocketExist(TEXT("AttackSocket")))
	{
		AttackPoint->SetupAttachment(
			GetMesh(),
			TEXT("AttackSocket")
		);
	}
	
	AttackPoint->SetCollisionProfileName(TEXT("EnemyAttack"));
	AttackPoint->SetGenerateOverlapEvents(true);

	
	SetActiveAttackCollision(false);
}

void AGasTank::BeginPlay()
{
	Super::BeginPlay();

	JetBalloonComponent = NewObject<UJetBalloonComponent>(this);
	AddOwnedComponent(JetBalloonComponent);
	JetBalloonComponent->RegisterComponent();
}

void AGasTank::Tick(float DeltaSeconds)
{
	if (IsExplosion)
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

			this->Destroy();
		}
	}
	else
	{
		Super::Tick(DeltaSeconds);
	}

}

void AGasTank::OnDead()
{
	if (GasTankDurablity <= 0)
	{
		IsExplosion = true;
	}
	
	Super::OnDead();
}

void AGasTank::OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param)
{
	if (GasTankDurablity > 0 && Param->HitResult.Component == GasCylinder)
	{
		GasTankDurablity -= 1;
		if (GasTankDurablity <= 0)
		{
			FSM->ChangeState(EEnemyState::DEATH, this);
		}
	}
	else
	{
		Super::OnTriggerEnter(OtherActor, Param);
	}
}

void AGasTank::OnTriggerAttack(bool Start)
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
					50,
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
		
		if (MontageMap.Contains("Attack"))
		{
			PlayAnimMontage(MontageMap["Attack"], 1.f, "Attack");
		}
		
		return;
	}
	// SetActiveAttackCollision(false);
	FSM->ChangeState(EEnemyState::IDLE, this);
}

void AGasTank::SetActiveAttackCollision(bool Active) const
{
	AttackPoint->SetVisibility(Active);
	AttackPoint->SetCollisionEnabled(Active? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}
