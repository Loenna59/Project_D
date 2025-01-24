// Fill out your copyright notice in the Description page of Project Settings.


#include "JetBalloonComponent.h"

#include "BaseZombie.h"
#include "GameDebug.h"

// Sets default values for this component's properties
UJetBalloonComponent::UJetBalloonComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UJetBalloonComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UJetBalloonComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UJetBalloonComponent::StartSimulate(USkeletalMeshComponent* Mesh)
{
	if (!Mesh)
	{
		return;
	}

	// GameDebug::ShowDisplayLog(GetWorld(), "StartSimulate");

	Mesh->SetSimulatePhysics(true);

	FVector RandomImpulse = FVector(
					FMath::FRandRange(-100.f, 100.f),
					FMath::FRandRange(-100.f, 100.f),
					FMath::FRandRange(500.f, 100.f) // 위쪽 방향 힘 추가
				);
			
	FVector ImpulseLocation = Mesh->GetComponentLocation().UpVector;
				
	// 전체 SkeletalMesh에 Impulse 적용 (튀어오르는 효과)
	Mesh->AddImpulseAtLocation(RandomImpulse, ImpulseLocation);
			
	// 특정 뼈에도 추가적인 Force 적용 (예: pelvis 아래쪽)
	FVector BoneForce = FVector(
		FMath::FRandRange(-500.f, 500.f),
		FMath::FRandRange(-500.f, 500.f),
		FMath::FRandRange(200.f, 1000.f)
	);
	Mesh->AddForceToAllBodiesBelow(BoneForce, "pelvis", true, true);
}



