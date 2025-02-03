// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "DemolisherProp.h"
#include "Demolisher.generated.h"

UCLASS()
class PROJECT_D_API ADemolisher : public ABaseZombie
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	float CurrentChargeSpeed = 0.f;
	
	FTimerHandle ChargingTimerHandle;
	FTimerHandle AttackTimerHandle2;

public:
	ADemolisher();

	virtual void OnStartAttack() override;

	virtual void FinishAttack() override;

	virtual void PhysicsAttack(AZombieTriggerParam* ZombieParam, FHitResult HitResult, USkeletalMeshComponent* MeshComponent, bool& IsSimulated) override;
	
	void Throw();
	
	void ChargeTo(float Speed, float Acceleration);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FSM")
	float ThrowDuration = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FSM")
	float ChargeSpeed = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FSM")
	float ChargeAcceleration = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackTiming2 = 1.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* AttackPoint2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MidRangeAttackRadius = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADemolisherProp> PropFactory;
};
