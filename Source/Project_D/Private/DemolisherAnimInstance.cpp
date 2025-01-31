// Fill out your copyright notice in the Description page of Project Settings.


#include "DemolisherAnimInstance.h"

void UDemolisherAnimInstance::SetChargingAttack(bool IsCharging)
{
	if (IsCharging)
	{
		bIsSprint = true;
		return;
	}

	bIsSprint = false;
	bIsWalking = false;
}
