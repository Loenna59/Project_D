// Fill out your copyright notice in the Description page of Project Settings.


#include "ForceActor.h"

// Sets default values
AForceActor::AForceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RadialForce = CreateDefaultSubobject<URadialForceComponent>("RadialForce");

}

// Called when the game starts or when spawned
void AForceActor::BeginPlay()
{
	Super::BeginPlay();

	if (RadialForce)
	{
		RadialForce->FireImpulse();
	}
	
}

// Called every frame
void AForceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

