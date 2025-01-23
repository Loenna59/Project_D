// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TitleUI.h"

#include "PlayerCharacter.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UTitleUI::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &UTitleUI::OnPlayGame);
	QuitButton->OnClicked.AddDynamic(this, &UTitleUI::OnQuitGame);
	
}

void UTitleUI::OnPlayGame()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetShowMouseCursor(false);
	UGameplayStatics::OpenLevel(GetWorld(), GameLevelName);
}

void UTitleUI::OnQuitGame()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
}
