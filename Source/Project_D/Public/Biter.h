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

	void SetActiveAttackCollision(bool Active) const;

	UFUNCTION()
	void OnOverlappedAttackPoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

};
