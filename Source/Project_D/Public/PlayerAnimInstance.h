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
	UPROPERTY()
	UActionComponent* ActionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|States")
	EPlayerState PlayerState = EPlayerState::WalkingOnGround;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|States")
	EPlayerHandState PlayerHandState = EPlayerHandState::Hand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|States")
	bool bShouldMove = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|States")
	bool bIsFalling = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|States")
	bool bIsDead = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|Values")
	FVector2D MovementVector = FVector2D::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|Values")
	FVector Velocity = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|Values")
	float GroundSpeed = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|Values")
	FRotator AimRotation = FRotator::ZeroRotator;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|Fall")
	float MinHardFallVelocity = 1000.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Binded Data|Fall")
	float MaxSurviveFallVelocity = 2000.0f;
	
	UFUNCTION()
	void AnimNotify_OnDropkickImpact() const;
	UFUNCTION()
	void AnimNotify_OnStandingKickImpact() const;

public:
	UFUNCTION()
	void ProceduralRecoil(float Multiplier);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform Recoil = FTransform::Identity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform RecoilTransform = FTransform::Identity;
};
