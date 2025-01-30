// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieFSMComponent.h"
#include "DemolisherFSMComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UDemolisherFSMComponent : public UZombieFSMComponent
{
	GENERATED_BODY()

public:
	virtual void SetupState(ABaseZombie* Zombie) override;
	
};
