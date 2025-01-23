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

	virtual void OnDead() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* GasCylinder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UJetBalloonComponent* JetBalloonComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ExplosionVFX;

	// UFUNCTION()
	virtual void OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param) override;

	bool IsExplosion = false;

	float DeadTime = 0;
	float DelayDeadTime = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GasTankDurablity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* AttackPoint;

	virtual void OnTriggerAttack(bool Start) override;

	void SetActiveAttackCollision(bool Active) const;
	
	FTimerHandle AttackTimerHandle;
	float AttackTiming = 1.f;
};
