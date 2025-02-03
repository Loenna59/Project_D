// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameOverUI.h"

#include "Kismet/GameplayStatics.h"

void UGameOverUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TWeakObjectPtr<UGameOverUI> GameOverUI = this;

	// 3 + 4초 뒤에 UI를 그만 표시할거임.
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[GameOverUI]()
		{
			if (GameOverUI.IsValid())
			{
				UGameplayStatics::OpenLevel(GameOverUI->GetWorld(), TEXT("TitleMap"));
			}
		},
		7.0f,
		false
	);
}
