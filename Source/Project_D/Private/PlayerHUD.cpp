// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "Components/ProgressBar.h"
#include "Components/StackBox.h"
#include "Components/TextBlock.h"

void UPlayerHUD::OnChangeHp(const int Hp, const int MaxHp)
{
	UE_LOG(LogTemp, Warning, TEXT("%d / %d"), Hp, MaxHp);
	TextHp->SetText(FText::FromString(FString::Printf(TEXT("%d"), Hp)));
	
	ProgressBarHp->SetPercent(FMath::Max(0, static_cast<float>(Hp) / MaxHp));
}

void UPlayerHUD::OnZombieCleared()
{
	UE_LOG(LogTemp, Display, TEXT("UPlayerHUD::OnZombieCleared"));
	if (BP_DemolisherHealthUI)
	{
		BP_DemolisherHealthUI->SetVisibility(ESlateVisibility::Hidden);
	}
	StackBox->RemoveChildAt(1);
}