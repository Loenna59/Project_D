// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultGameModeBase.h"

#include "GameDebug.h"

void AVaultGameModeBase::IncreaseCount()
{
	ZombieCount++;
	// GameDebug::ShowDisplayLog(GetWorld(), FString::FromInt(ZombieCount));
}

void AVaultGameModeBase::DecreaseCount()
{
	ZombieCount--;
	// GameDebug::ShowDisplayLog(GetWorld(), FString::FromInt(ZombieCount));
	if (ZombieCount <= 0)
	{
		// 1초 뒤에 클리어 UI 출력
	}
}
