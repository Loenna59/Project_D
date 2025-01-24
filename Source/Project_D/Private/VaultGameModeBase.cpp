// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultGameModeBase.h"

#include "GameDebug.h"
#include "UI/GameClearUI.h"

void AVaultGameModeBase::IncreaseCount()
{
	ZombieCount++;
	// GameDebug::ShowDisplayLog(GetWorld(), FString::FromInt(ZombieCount));
}

void AVaultGameModeBase::DecreaseCount()
{
	ZombieCount--;

	FString Str = FString::Printf(TEXT("Number of Zombie: %d"), ZombieCount);
	
	GameDebug::ShowDisplayLog(GetWorld(), Str);
	if (ZombieCount <= 0)
	{
		UGameClearUI* GameClearUI = Cast<UGameClearUI>(CreateWidget(GetWorld(), UIFactory));
		// 3초 뒤에...
		FTimerHandle TimerHandle;

		APostProcessVolume* PostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass());
		if (PostProcessVolume)
		{
			PostProcessVolume->bUnbound = true;
			PostProcessVolume->Settings.MotionBlurAmount = 10.0f;

			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[GameClearUI, PostProcessVolume]()
				{
					if (PostProcessVolume && PostProcessVolume->IsValidLowLevel())
					{
						PostProcessVolume->Destroy();
					}
					
					if (GameClearUI)
					{
						GameClearUI->AddToViewport();
					}
				},
				3.0f,
				false
			);
		}
		
	}
}
