// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimNotifyState.h"

#include "PlayerCharacter.h"

void UPlayerAnimNotifyState::BeginHardLanding(APlayerCharacter* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimNotifyState::BeginHardLanding"));
	Player->bIsHardLanding = true;
}

void UPlayerAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimNotifyState::NotifyBegin %s"), *NotifyName);
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	APlayerCharacter* Player = nullptr;
	if (MeshComp && MeshComp->GetOwner())
	{
		Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	}
	if (nullptr == Player)
	{
		return;
	}

	if (NotifyName == "HardLanding")
	{
		BeginHardLanding(Player);
	}
	else if (NotifyName == "SafetyLanding")
	{
		// BeginSafetyLanding(Player);
	}
}

void UPlayerAnimNotifyState::TickSafetyLanding(APlayerCharacter* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimNotifyState::TickSafetyLanding"));
	Player->bIsHardLanding = true;
}

void UPlayerAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimNotifyState::NotifyEnd %s"), *NotifyName);
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	APlayerCharacter* Player = nullptr;
	if (MeshComp && MeshComp->GetOwner())
	{
		Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	}
	if (nullptr == Player)
	{
		return;
	}
	
	if (NotifyName == "HardLanding")
	{
		// TickHardLanding(Player);
	}
	else if (NotifyName == "SafetyLanding")
	{
		TickSafetyLanding(Player);
	}
}

void UPlayerAnimNotifyState::EndHardLanding(APlayerCharacter* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimNotifyState::EndHardLanding"));
	Player->bIsHardLanding = false;
}

void UPlayerAnimNotifyState::EndSafetyLanding(APlayerCharacter* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimNotifyState::EndSafetyLanding"));
	Player->bIsHardLanding = false;
}

void UPlayerAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimNotifyState::NotifyEnd %s"), *NotifyName);
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	APlayerCharacter* Player = nullptr;
	if (MeshComp && MeshComp->GetOwner())
	{
		Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	}
	if (nullptr == Player)
	{
		return;
	}

	if (NotifyName == "HardLanding")
	{
		EndHardLanding(Player);
	}
	else if (NotifyName == "SafetyLanding")
	{
		EndSafetyLanding(Player);
	}
}
