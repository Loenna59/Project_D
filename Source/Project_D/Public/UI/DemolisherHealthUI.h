// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "DemolisherHealthUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UDemolisherHealthUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextHp;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBarHp;
	
	void OnChangeHp(int Hp, int MaxHp);
};
