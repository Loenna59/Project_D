// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "Components/ProgressBar.h"
#include "Components/StackBox.h"
#include "Components/TextBlock.h"

void UPlayerHUD::OnChangeHp(const int Hp, const int MaxHp)
{
	UE_LOG(LogTemp, Warning, TEXT("%d / %d"), Hp, MaxHp);
	TextHp->SetText(FText::FromString(FString::Printf(TEXT("%d"), Hp)));
	
	ProgressBarHp->SetPercent(static_cast<float>(Hp) / MaxHp);
}

void UPlayerHUD::OnZombieCleared()
{
	StackBox->RemoveChildAt(1);
}