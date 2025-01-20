// Fill out your copyright notice in the Description page of Project Settings.


#include "GasTank.h"

#include "ExplosiveCollisionActor.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceActor.h"


AGasTank::AGasTank()
{
	PrimaryActorTick.bCanEverTick = true;
	
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
	//Super::Tick(DeltaSeconds);

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

}

void AGasTank::OnDead()
{
	IsExplosion = true;
	//Super::OnDead();
}
