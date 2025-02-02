// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/BloodDecalActor.h"

void ABloodDecalActor::BeginPlay()
{
	Super::BeginPlay();

	TWeakObjectPtr<ABloodDecalActor> WeakThis = this;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[WeakThis] ()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->Destroy();
			}
		},
		LifeTime,
		false
	);
}
