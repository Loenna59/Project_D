// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ZombieAnimInstance.h"

void UZombieAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UZombieAnimInstance::PlayMontage(AAIController* AIController, AnimState MontageState, TFunction<void(float PlayLength)> Callback)
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
		bMontagePlaying = true;
		CurrentMontage = MontageMap[MontageState];
		CurrentState = MontageState;
		Montage_Play(CurrentMontage, 1.5f);

		if (Callback)
		{
			Callback(CurrentMontage->GetPlayLength() / 1.5f);
		}

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UZombieAnimInstance::OnMontageEnded);
	
		Montage_SetEndDelegate(EndDelegate, CurrentMontage);
	}
}

void UZombieAnimInstance::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bMontagePlaying = false;
	CurrentMontage = nullptr;
	CurrentState = AnimState::None;
}

bool UZombieAnimInstance::GetRandomBoolThreadSafe()
{
	return FMath::RandBool();
}
