// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultGameModeBase.h"

#include "PlayerCharacter.h"
#include "PlayerHUD.h"

void AVaultGameModeBase::IncreaseCount()
{
	ZombieCount++;
	// GameDebug::ShowDisplayLog(GetWorld(), FString::FromInt(ZombieCount));
}

void AVaultGameModeBase::DecreaseCount()
{
	ZombieCount--;
	UE_LOG(LogTemp, Display, TEXT("Number of Zombie: %d"), ZombieCount);

	// 미션 목록에서 좀비 소탕 미션을 삭제
	if (ZombieCount <= 0)
	{
		if (const APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner()))
		{
			if (Player->PlayerHUD)
			{
				Player->PlayerHUD->OnZombieCleared();
			}
		};
	}
	// if (ZombieCount <= 0)
	// {
	// 	UGameClearUI* GameClearUI = Cast<UGameClearUI>(CreateWidget(GetWorld(), UIFactory));
	// 	// 3초 뒤에...
	// 	FTimerHandle TimerHandle;
	//
	// 	APostProcessVolume* PostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass());
	// 	if (PostProcessVolume)
	// 	{
	// 		PostProcessVolume->bUnbound = true;
	// 		PostProcessVolume->Settings.MotionBlurAmount = 10.0f;
	//
	// 		GetWorld()->GetTimerManager().SetTimer(
	// 			TimerHandle,
	// 			[GameClearUI, PostProcessVolume]()
	// 			{
	// 				if (PostProcessVolume && PostProcessVolume->IsValidLowLevel())
	// 				{
	// 					PostProcessVolume->Destroy();
	// 				}
	// 				
	// 				if (GameClearUI)
	// 				{
	// 					GameClearUI->AddToViewport();
	// 				}
	// 			},
	// 			3.0f,
	// 			false
	// 		);
	// 	}
	// 	
	// }
}
