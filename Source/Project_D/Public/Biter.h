// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "Biter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API ABiter : public ABaseZombie
{
	GENERATED_BODY()

protected:
	virtual void SetupInternal() override;
	
	virtual FName RenameBoneName(const FName& HitBoneName) override;

	virtual bool IsPhysicsBone(const FName& HitBoneName) override;
	
};
