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
	class UJetBalloonComponent* JetBalloonComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ExplosionVFX;

	bool IsExplosion = false;

	float DeadTime = 0;
	float DelayDeadTime = 2.f;
};
