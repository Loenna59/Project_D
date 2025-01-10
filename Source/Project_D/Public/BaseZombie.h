// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EEnemyState.h"
#include "ZombieFSMComponent.h"
#include "GameFramework/Character.h"
#include "BaseZombie.generated.h"

UCLASS()
class PROJECT_D_API ABaseZombie : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseZombie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemyState EState = EEnemyState::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> BoneDurability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> BrokenBones;

	UPROPERTY(EditAnywhere)
	FName HeadBone;

	UPROPERTY(EditAnywhere)
	FName RightHandBone;

	UPROPERTY(EditAnywhere)
	FName LeftHandBone;

	UPROPERTY(EditAnywhere)
	TArray<FName> BoneArray_L;
	
	UPROPERTY(EditAnywhere)
	TArray<FName> BoneArray_R;
	
	UPROPERTY(EditAnywhere)
	AActor* Attacker;

	UPROPERTY()
	class UZombieFSMComponent* FSM;
	
	UFUNCTION()
	virtual void AnyDamage(int32 Damage, const FName& HitBoneName, class AActor* DamageCauser);

	virtual bool ContainsBrokenBones(TArray<FName> BoneNames);
	
protected:
	virtual bool IsPhysicsBone(const FName& HitBoneName);
	
	virtual void SetupInternal();
	
	virtual FName RenameBoneName(const FName& HitBoneName);
	
	virtual bool ApplyDamageToBone(const FName& HitBoneName, int32 Damage);

	virtual void Dismemberment(const FName& HitBoneName);

	virtual void ApplyPhysics(const FName& HitBoneName);

	virtual FVector CalculateImpulse();

	virtual bool LineTraceChannel(struct FHitResult& HitResult, FVector Start, FVector End);
};
