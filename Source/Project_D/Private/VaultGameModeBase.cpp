// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultGameModeBase.h"

#include "PlayerCharacter.h"
#include "PlayerHUD.h"
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
	UE_LOG(LogTemp, Display, TEXT("Number of Zombie: %d"), ZombieCount);

	// 미션 목록에서 좀비 소탕 미션을 삭제
	/*
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
	*/

	if (ZombieCount <= 0)
	{
		if (bIsAppearDemolisher)
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
		else
		{
			bIsAppearDemolisher = true;
			
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
			}
			
		}
		
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
