// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionTrigger.h"
#include "EEnemyState.h"
#include "ZombieFSMComponent.h"
#include "GameFramework/Character.h"
#include "BaseZombie.generated.h"

UCLASS()
class PROJECT_D_API ABaseZombie : public ACharacter, public ICollisionTrigger
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> BoneDurability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> BrokenBones;

	TArray<FName> WeaknessBones;

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

	UPROPERTY(EditAnywhere)
	AActor* DetectedTarget;

	UPROPERTY(EditAnywhere)
	class UZombieFSMComponent* FSM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRadius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAttacking = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, class UAnimMontage*> MontageMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp = 100;
	
	virtual bool ContainsBrokenBones(TArray<FName> BoneNames);

	virtual void OnTriggerAttack(bool Start);

	virtual void OnDead();

	UFUNCTION()
	virtual void OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param) override;

	UFUNCTION()
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	//PathFinding
	UPROPERTY()
	class APathFindingBoard* PathFindingBoard;

	UPROPERTY()
	class APathField* FromPathField;
	UPROPERTY()
	class APathField* ToPathField;

	FVector FromLocation;
	FVector ToLocation;

	class APathField* GetPlacedPathField();
	bool MoveNextField(APathField* Start);

protected:
	virtual bool IsPhysicsBone(const FName& HitBoneName);
	
	virtual void SetupInternal();
	
	virtual FName RenameBoneName(const FName& HitBoneName);
	
	virtual bool ApplyDamageToBone(const FName& HitBoneName, int32 Damage);

	virtual void Dismemberment(const FName& HitBoneName);

	virtual void ApplyPhysics(const FName& HitBoneName);

	virtual FVector CalculateImpulse();

	virtual bool InstantKilled(const FName& HitBoneName);

};
