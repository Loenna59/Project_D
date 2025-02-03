// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UStackBox;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class PROJECT_D_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextHp;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBarHp;
	UPROPERTY(meta=(BindWidget))
	UStackBox* StackBox;
	UPROPERTY(meta=(BindWidget))
	UUserWidget* BP_DemolisherHealthUI;
	
	void OnChangeHp(int Hp, int MaxHp);
	void OnZombieCleared();
};