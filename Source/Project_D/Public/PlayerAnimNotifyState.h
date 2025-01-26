// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PlayerAnimNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UPlayerAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notify")
	FString NotifyName;
	
	static void BeginHardLanding(APlayerCharacter* Player);
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	static void TickSafetyLanding(APlayerCharacter* Player);
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	static void EndHardLanding(APlayerCharacter* Player);
	static void EndSafetyLanding(APlayerCharacter* Player);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
