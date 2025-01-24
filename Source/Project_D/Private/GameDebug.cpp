// Fill out your copyright notice in the Description page of Project Settings.


#include "GameDebug.h"

#include "Kismet/KismetSystemLibrary.h"

GameDebug::GameDebug()
{
}

GameDebug::~GameDebug()
{
}

void GameDebug::ShowDisplayLog(const UWorld* World, const FString& Message, bool WithConsole)
{
	// UKismetSystemLibrary::PrintString(World, Message, true, WithConsole);
}
