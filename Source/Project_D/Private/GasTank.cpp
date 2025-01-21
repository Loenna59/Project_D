// Fill out your copyright notice in the Description page of Project Settings.


#include "GasTank.h"

#include "ExplosiveCollisionActor.h"
#include "GameDebug.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceActor.h"


AGasTank::AGasTank()
{
	PrimaryActorTick.bCanEverTick = true;

	GasCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GasCylinder"));
	GasCylinder->SetupAttachment(GetMesh());
	GasCylinder->SetCollisionProfileName("Enemy");
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
