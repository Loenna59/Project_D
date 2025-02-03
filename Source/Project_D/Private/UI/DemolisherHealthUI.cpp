// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DemolisherHealthUI.h"

void UDemolisherHealthUI::OnChangeHp(int Hp, int MaxHp)
{
	TextHp->SetText(FText::FromString(FString::Printf(TEXT("%d"), Hp)));
	
	ProgressBarHp->SetPercent(FMath::Max(0, static_cast<float>(Hp) / MaxHp));
}
