// Fill out your copyright notice in the Description page of Project Settings.


#include "FallSafetyZone.h"

// Sets default values
AFallSafetyZone::AFallSafetyZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(
		TEXT("/Script/Engine.StaticMesh'/Game/Assets/Environments/pile-of-old-tires/source/tires2.tires2'"));
	if (StaticMeshAsset.Object)
	{
		Mesh->SetStaticMesh(StaticMeshAsset.Object);
	}

	Mesh->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
}

// Called when the game starts or when spawned
void AFallSafetyZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFallSafetyZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}