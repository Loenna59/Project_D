// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "Demolisher.generated.h"

UENUM(BlueprintType)
enum class DemolisherAttackPattern : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Throw UMETA(DisplayName = "Throw"),
	Swing UMETA(DisplayName = "Swing"),
	Charging UMETA(DisplayName = "Charging"),
};

UCLASS()
class PROJECT_D_API ADemolisher : public ABaseZombie
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	void SetActiveAttackCollision(bool Active);

public:
	ADemolisher();

	virtual void FinishAttack() override;
	
	void Throw();
	
	void ChargeTo(float Speed, float Acceleration);

	float ChargeSpeed = 0.f;
	FTimerHandle ChargingTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MidRangeAttackRadius = 800.f;
};
