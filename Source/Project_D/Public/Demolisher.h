// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "Demolisher.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API ADemolisher : public ABaseZombie
{
	GENERATED_BODY()

public:
	ADemolisher();

	virtual void OnTriggerAttack(bool Start) override;
	
	void Throw();
	void Swing();
	void ChargeTo();
};
