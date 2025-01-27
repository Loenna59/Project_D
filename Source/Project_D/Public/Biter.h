// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "Biter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API ABiter : public ABaseZombie
{
	GENERATED_BODY()
	
// protected:
// 	virtual void SetupInternal() override;
// 	
// 	virtual FName RenameBoneName(const FName& HitBoneName) override;
//
// 	virtual bool IsPhysicsBone(const FName& HitBoneName) override;

	virtual void BeginPlay() override;

public:
	ABiter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* LeftArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* RightArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* LeftLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* RightLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* AttackPoint;

	virtual void OnTriggerAttack(bool Start) override;

	void SetActiveAttackCollision(bool Active) const;

	UFUNCTION()
	void OnOverlappedAttackPoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	FTimerHandle AttackTimerHandle;
	float AttackTiming = 1.f;

};
