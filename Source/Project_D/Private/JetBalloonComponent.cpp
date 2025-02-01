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

	// 충격량 = mass * v - mass * v0

	// GameDebug::ShowDisplayLog(GetWorld(), "StartSimulate");
	Mesh->SetSimulatePhysics(true);

	// FVector RandomImpulse = FVector(
	// 	FMath::FRandRange(-0.2f, 0.2f),
	// 	FMath::FRandRange(-0.2f, 0.2f),
	// 	FMath::FRandRange(2.f, 4.f) // 위쪽 방향 힘 추가
	// );
	// 		
	// FVector ImpulseLocation = Mesh->GetComponentLocation().UpVector;
	// 			
	// // 전체 SkeletalMesh에 Impulse 적용 (튀어오르는 효과)
	// Mesh->AddImpulseAtLocation(RandomImpulse, ImpulseLocation);
	// 		
	// // 특정 뼈에도 추가적인 Force 적용 (예: pelvis 아래쪽)
	// FVector BoneForce = FVector(
	// 	FMath::FRandRange(-0.5f, 0.5f),
	// 	FMath::FRandRange(-0.5f, 0.5f),
	// 	FMath::FRandRange(1.f, 2.f)
	// );
	//
	// Mesh->AddImpulseToAllBodiesBelow(BoneForce, "Spine1", true, true);
}



