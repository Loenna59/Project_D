// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameClearUI.h"

#include "Kismet/GameplayStatics.h"

void UGameClearUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TWeakObjectPtr<UGameClearUI> WeakUI = this;
	
	// 3 + 4초 뒤에 UI를 그만 표시할거임.
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[WeakUI]()
		{
			if (WeakUI.IsValid())
			{
				UGameplayStatics::OpenLevel(WeakUI->GetWorld(), TEXT("TitleMap"));
			}
		},
		7.0f,
		false
	);
}
