// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultGameModeBase.h"

#include "Demolisher.h"
#include "GameDebug.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Kismet/GameplayStatics.h"
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

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("SpawnPoint"), FoundActors);

		if (FoundActors.Num() > 0)
		{
			DemolisherSpawnPoint = FoundActors[0];

			// 시퀀스 시작
			FMovieSceneSequencePlaybackSettings PlaybackSettings;
			ALevelSequenceActor* OutActor;
			
			ULevelSequencePlayer* LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), DemolisherSequence, PlaybackSettings, OutActor);
			if (LevelSequencePlayer->IsValidLowLevel())
			{
				LevelSequencePlayer->Play();
				LevelSequencePlayer->OnFinished.AddDynamic(this, &AVaultGameModeBase::OnSequenceFinished);
			}

			// 데몰리셔 스폰
			// 보스 hp UI 출력
			// 보스전 시작
		}
		
		// UGameClearUI* GameClearUI = Cast<UGameClearUI>(CreateWidget(GetWorld(), UIFactory));
		// // 3초 뒤에...
		// FTimerHandle TimerHandle;
		//
		// APostProcessVolume* PostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass());
		// if (PostProcessVolume)
		// {
		// 	PostProcessVolume->bUnbound = true;
		// 	PostProcessVolume->Settings.MotionBlurAmount = 10.0f;
		//
		// 	GetWorld()->GetTimerManager().SetTimer(
		// 		TimerHandle,
		// 		[GameClearUI, PostProcessVolume]()
		// 		{
		// 			if (PostProcessVolume && PostProcessVolume->IsValidLowLevel())
		// 			{
		// 				PostProcessVolume->Destroy();
		// 			}
		// 			
		// 			if (GameClearUI)
		// 			{
		// 				GameClearUI->AddToViewport();
		// 			}
		// 		},
		// 		3.0f,
		// 		false
		// 	);
		// }
		
	}
}

void AVaultGameModeBase::OnSequenceFinished()
{
	if (!DemolisherSpawnPoint)
	{
		return;
	}

	ADemolisher* Demolisher = GetWorld()->SpawnActor<ADemolisher>(DemolisherFactory, DemolisherSpawnPoint->GetActorTransform());
}
