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

	static void ShowDisplayLog(const UWorld* World, const FString& Message, bool WithConsole = false);
};
