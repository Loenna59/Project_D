// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "ZombieAnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class AnimState :uint8
{
	None,
	Idle,
	Moving,
	Attack,
	Hit,
	Crawl,
	Dead
};


UCLASS()
class PROJECT_D_API UZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	virtual void NativeBeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<AnimState, class UAnimMontage*> MontageMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMontagePlaying = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWalking;

	UPROPERTY()
	AnimState CurrentState = AnimState::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* CurrentMontage;

	UFUNCTION()
	void PlayMontage(AAIController* AIController, AnimState MontageState);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageFinished();
	
};
