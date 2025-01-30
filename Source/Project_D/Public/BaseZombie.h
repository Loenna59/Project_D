// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionTrigger.h"
#include "EBodyPart.h"
#include "PathVector.h"
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

	virtual void SetCollisionPartMesh(USkeletalMeshComponent* Part);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, EBodyPart> BoneRangeMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBodyPart, int32> BoneDurability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EBodyPart> BrokenParts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EBodyPart> WeaknessParts;
	
	UPROPERTY(EditAnywhere)
	AActor* Attacker;

	UPROPERTY(EditAnywhere)
	class UZombieFSMComponent* FSM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRadius = 200.f;

	UPROPERTY(EditAnywhere)
	bool bIsAttacking = false; 

	UPROPERTY(EditAnywhere)
	int32 CurrentHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass = 75.f;

	UPROPERTY(EditAnywhere)
	class UZombieAnimInstance* AnimationInstance;

	UPROPERTY(EditAnywhere)
	class UPathfindingComponent* Pathfinding;

	UPROPERTY()
	class AZombieAIController* AI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* AttackPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBodyPart, TObjectPtr<USkeletalMeshComponent>> PartMeshes;

	FTimerHandle AttackTimerHandle;
	
	float AttackTiming = 0.75f;

	int32 CurrentPathIndex = 0;

	float DistanceAlongSpline = 0.f;
	
	virtual bool ContainsBrokenBones(TArray<FName> BoneNames);

	virtual void OnTriggerAttack(bool Start);

	virtual void OnDead();

	UFUNCTION()
	virtual void OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param) override;

	UFUNCTION()
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Rotate();

	float CalculateDistanceToTarget() const;

	void FinishAttack();

protected:
	virtual bool IsPhysicsBone(EBodyPart Part);
	
	virtual bool ApplyDamageToBone(EBodyPart Part, int32 Damage);

	virtual void Dismemberment(EBodyPart Part);

	virtual FVector CalculateImpulse();

	virtual bool InstantKilled(EBodyPart Part);
};
