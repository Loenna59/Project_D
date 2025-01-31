// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
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
	Throw,
	Swing,
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDead;

	UPROPERTY()
	AnimState CurrentState = AnimState::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* CurrentMontage;
	
	void PlayMontage(
		AAIController* AIController,
		AnimState MontageState,
		TFunction<void(float PlayLength)> Callback
	);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
	bool GetRandomBoolThreadSafe();
};
