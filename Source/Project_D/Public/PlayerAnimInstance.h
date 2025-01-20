// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionComponent.h"
#include "PlayerAnimBlueprintInterface.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class UCharacterMovementComponent;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class PROJECT_D_API UPlayerAnimInstance : public UAnimInstance, public IPlayerAnimBlueprintInterface
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	virtual bool SetPlayerActionState(EActionState PlayerActionState) override;
	
	virtual bool SetMovementVector(const FVector2D& InMovementVector) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	APlayerCharacter* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	UCharacterMovementComponent* Movement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	bool bShouldMove = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	bool bIsAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EActionState PlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D MovementVector;
};
