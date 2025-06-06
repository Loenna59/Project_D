// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "JetBalloonComponent.h"
#include "GasTank.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API AGasTank : public ABaseZombie
{
	GENERATED_BODY()

public:
	AGasTank();
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* LeftArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* RightArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* LeftLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* RightLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* GasCylinder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UJetBalloonComponent* JetBalloonComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ExplosionVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundWave* DeadSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundWave* ExplosionSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundWave* HissingSFX;

	// UFUNCTION()
	virtual void OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param) override;

	virtual void OnDead() override;

	bool bIsExplosion = false;

	float DeadTime = 0;
	float DelayDeadTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GasTankDurablity = 1;

	FTimerHandle JetBalloonTimerHandle;
};
