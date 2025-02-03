// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CollisionTrigger.h"
#include "EBodyPart.h"
#include "PathVector.h"
#include "ZombieFSMComponent.h"
#include "ZombieTriggerParam.h"
#include "GameFramework/Character.h"
#include "UI/DemolisherHealthUI.h"
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

	virtual void Evaluate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, EBodyPart> BoneRangeMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBodyPart, int32> BoneDurability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EBodyPart> BrokenParts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EBodyPart> WeaknessParts;

	UPROPERTY(VisibleAnywhere)
	class UZombieFSMComponent* FSM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRadius = 200.f;
	
	bool bIsAttacking = false;
	bool bIsHitting = false;

	UPROPERTY()
	int32 CurrentHp;

	UPROPERTY()
	UDemolisherHealthUI* HealthUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass = 75.f;

	UPROPERTY()
	class UZombieAnimInstance* AnimationInstance;

	UPROPERTY(EditAnywhere)
	class UPathfindingComponent* Pathfinding;

	UPROPERTY()
	class AZombieAIController* AI;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* AttackPoint;

	UPROPERTY()
	TMap<EBodyPart, TObjectPtr<USkeletalMeshComponent>> PartMeshes;

	FTimerHandle AttackTimerHandle;
	FTimerHandle HitTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FSM")
	float AttackInterval = 1.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackTiming = 0.75f;

	int32 CurrentPathIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* BloodSplatter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABloodDecalActor> BloodDecalFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* BloodNiagaraSystem;

	virtual bool ContainsBrokenBones(TArray<FName> BoneNames);

	virtual void OnStartAttack();

	virtual void OnDead();

	virtual void PhysicsAttack(AZombieTriggerParam* ZombieParam, FHitResult HitResult, USkeletalMeshComponent* MeshComponent,
	                   bool& IsSimulated);

	UFUNCTION()
	virtual void OnTriggerEnter(AActor* OtherActor, ACollisionTriggerParam* Param) override;

	UFUNCTION()
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnMeshCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	float CalculateDistanceToTarget() const;

	virtual void FinishAttack();

	AAIController* GetAIController() const;

protected:
	virtual bool IsPhysicsBone(EBodyPart Part);
	
	virtual bool ApplyDamageToBone(EBodyPart Part, int32 Damage);

	virtual void Dismemberment(EBodyPart Part);

	virtual FVector CalculateImpulse();

	virtual bool InstantKilled(EBodyPart Part);
};
