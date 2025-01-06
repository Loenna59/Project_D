// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectRename.h"
#include "TestCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	NORMAL,
	CLAWING
};

UCLASS()
class PROJECT_D_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

	bool bDoOnce = false;
	bool bIsGateOpen = false;
	
	FTimerHandle OnceTimerHandle;
	FTimerHandle DelayTimerHandle;

protected:
	UPROPERTY()
	int32 AppliedDamage;

	UPROPERTY()
	FName HitBoneName;

	UPROPERTY()
	class AActor* DamageCauser;

	UPROPERTY()
	TArray<FName> BrokenBones;

	UPROPERTY()
	bool HasSpineBroken = false;

	UPROPERTY()
	EMovementState MovementZombie = EMovementState::NORMAL;

public:
	UPROPERTY(VisibleAnywhere)
	TMap<FName, int32> BoneDurability;

public:
	// Sets default values for this character's properties
	ATestCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Gate(bool bOpen);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void AnyDamage(int32 Damage, const FName& HitBoneName, class AActor* DamageCauser);
	
	void RenameBoneName();

	bool ApplyDamageToBone();

	void Dismemberment();

	void ChangeMovementType();

	void ApplyPhysics();

	void SetCapsuleLocation();

	void AddForceToBones();

	UFUNCTION(BlueprintCallable)
	FVector CalculateCrawlForce(float ForwardForce, float UpwardForce);
	
	UFUNCTION(BlueprintCallable)
	FVector CalculateImpulse();

	UFUNCTION(BlueprintCallable)
	FVector GetBoneSocketLocation();

	UFUNCTION(BlueprintCallable)
	bool FindBrokenBones(TArray<FName> BoneNames);
};
