// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ZombieAnimInstance.h"

#include "GameDebug.h"
#include "Animation/AnimMontage.h"

void UZombieAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UZombieAnimInstance::PlayMontage(AAIController* AIController, AnimState MontageState)
{
	if (bMontagePlaying && CurrentMontage)
	{
		if (FOnMontageEnded* EndDelegate = Montage_GetEndedDelegate(CurrentMontage))
		{
			EndDelegate->Execute(CurrentMontage, true);
		}
	}
	
	if (MontageMap.Contains(MontageState))
	{
		AIController->StopMovement();
		bMontagePlaying = true;
		CurrentMontage = MontageMap[MontageState];
		CurrentState = MontageState;
		Montage_Play(CurrentMontage, 1.5f);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UZombieAnimInstance::OnMontageEnded);
		Montage_SetEndDelegate(EndDelegate, CurrentMontage);	
	}
}

void UZombieAnimInstance::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	FString Str = FString::Printf(TEXT("%s [%d]"), *Montage->GetName(), bInterrupted);
	GameDebug::ShowDisplayLog(GetWorld(), Str, FColor::Cyan, true);
	bMontagePlaying = false;
	CurrentMontage = nullptr;
	CurrentState = AnimState::None;
}

void UZombieAnimInstance::OnMontageFinished()
{
}
