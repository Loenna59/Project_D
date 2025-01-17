// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveBarrelActor.h"

#include "ExplosiveCollisionActor.h"
#include "GameDebug.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceActor.h"

// Sets default values
AExplosiveBarrelActor::AExplosiveBarrelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	SetRootComponent(Collision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> BarrelMesh(TEXT("/Script/Engine.StaticMesh'/Game/Assets/Explosive_Barrel/explosive_barrel.explosive_barrel'"));

	if (BarrelMesh.Succeeded())
	{
		Mesh->SetStaticMesh(BarrelMesh.Object);
	}

	Mesh->SetRelativeScale3D(FVector::OneVector * 0.5f);

}

// Called when the game starts or when spawned
void AExplosiveBarrelActor::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentHit.AddDynamic(this, &AExplosiveBarrelActor::OnCollisionHit);
}

// Called every frame
void AExplosiveBarrelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosiveBarrelActor::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void AExplosiveBarrelActor::OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param)
{
	if (UWorld* const World = GetWorld())
	{
		check(ExplosionVFX);
		GameDebug::ShowDisplayLog(World, "AExplosiveBarrelActor::OnCollisionHit");

		FTransform SpawnTransform(FRotator::ZeroRotator, GetActorLocation(), FVector::OneVector);

		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			ExplosionVFX,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector::OneVector * 4.f
		);

		GetWorld()->SpawnActor<AExplosiveCollisionActor>(AExplosiveCollisionActor::StaticClass(), SpawnTransform);
		GetWorld()->SpawnActor<ARadialForceActor>(ARadialForceActor::StaticClass(), SpawnTransform);

		this->Destroy();
	}
}