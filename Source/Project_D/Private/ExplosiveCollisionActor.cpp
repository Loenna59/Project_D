// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveCollisionActor.h"

// Sets default values
AExplosiveCollisionActor::AExplosiveCollisionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->SetSphereRadius(500.f);
	Collision->SetCollisionProfileName("BlockAll");
}

// Called when the game starts or when spawned
void AExplosiveCollisionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosiveCollisionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTime > 1.f)
	{
		this->Destroy();
	}
	
	CurrentTime += DeltaTime;
}

