// Fill out your copyright notice in the Description page of Project Settings.


#include "DemolisherProp.h"

#include "GameDebug.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

ADemolisherProp::ADemolisherProp()
{
	bAllowTickBeforeBeginPlay = true;
}

void ADemolisherProp::BeginPlay()
{
	Super::BeginPlay();

	UGeometryCollectionComponent* GCC = GetGeometryCollectionComponent();

	if (GCC)
	{
		GCC->OnComponentHit.AddDynamic(this, &ADemolisherProp::OnCollisionHit);
	}
}

void ADemolisherProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADemolisherProp::Fire(FVector StartLocation, FVector TargetLocation)
{
	UGeometryCollectionComponent* GCC = GetGeometryCollectionComponent();
	
	if (GCC)
	{
		GCC->SetSimulatePhysics(true);
		GCC->SetNotifyRigidBodyCollision(true); // 충돌 이벤트 활성화

		bool bSuggest = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
			this,
			Velocity,
			StartLocation,
			TargetLocation,
			GetWorld()->GetGravityZ(),
			0.5f
		);

		if (bSuggest && bShowDebug)
		{
			FPredictProjectilePathParams PredictParams(10.0f, StartLocation, Velocity, 10.0f);
			PredictParams.DrawDebugTime = 10.0f;     
			PredictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;
			PredictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
			FPredictProjectilePathResult result;
			UGameplayStatics::PredictProjectilePath(this, PredictParams, result);
		}

		GCC->AddImpulse(Velocity, NAME_None, true);
	}
}

void ADemolisherProp::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// GameDebug::ShowDisplayLog(GetWorld(), "OnCollisionHit", FColor::Orange);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, Hit.Location);
	
	FTimerHandle TimerHandle;

	TWeakObjectPtr<ADemolisherProp> WeakThis = this;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		[WeakThis] ()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->Destroy();
			}
		},
		5.0f,
		false
	);
}
