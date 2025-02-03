// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VaultGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API AVaultGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UGameClearUI> UIFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADemolisher> DemolisherFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ULevelSequence* DemolisherSequence;
	
	int32 ZombieCount = 0;

	UPROPERTY()
	bool bIsAppearDemolisher;

	UPROPERTY()
	AActor* DemolisherSpawnPoint;

	void IncreaseCount();
	void DecreaseCount();

	UFUNCTION()
	void OnSequenceFinished();
	
};
