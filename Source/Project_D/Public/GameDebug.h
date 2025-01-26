// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECT_D_API GameDebug
{
public:
	GameDebug();
	~GameDebug();

	static void ShowDisplayLog(const UWorld* World, const FString& Message, FLinearColor Color = FLinearColor(0.0f, 0.66f, 1.0f), bool WithConsole = false);
};
