// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "DemolisherProp.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API ADemolisherProp : public AGeometryCollectionActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	FVector Velocity;

	bool bLaunch = false;
	
public:
	ADemolisherProp();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 1000;
	
	void Fire(FVector StartLocation, FVector TargetLocation);

	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
