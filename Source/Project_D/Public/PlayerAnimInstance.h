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

	UPROPERTY()
	APlayerCharacter* Player = nullptr;
	UPROPERTY()
	UCharacterMovementComponent* Movement = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	float GroundSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	bool bShouldMove = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Essential Movement Data")
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPlayerState PlayerState = EPlayerState::WalkingOnGround;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D MovementVector = FVector2D::ZeroVector;

public:
	virtual bool SetPlayerActionState(EPlayerState PlayerActionState) override;
};
