// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleMode.h"

#include "Blueprint/UserWidget.h"
#include "UI/TitleUI.h"

void ATitleMode::BeginPlay()
{
	Super::BeginPlay();

	ensure(UIFactory);
	
	UUserWidget* UI = CreateWidget<UTitleUI>(GetWorld(), UIFactory, TEXT("TitleUI"));
	UI->AddToViewport();
}
