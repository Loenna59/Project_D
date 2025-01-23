// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_D_API UTitleUI : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* StartButton;

	UPROPERTY(meta=(BindWidget))
	class UButton* QuitButton;

	UFUNCTION()
	void OnPlayGame();

	UFUNCTION()
	void OnQuitGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GameLevelName = TEXT("PlayerTestMap");
};
