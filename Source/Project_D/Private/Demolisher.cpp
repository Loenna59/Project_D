// Fill out your copyright notice in the Description page of Project Settings.


#include "Demolisher.h"

#include "GameDebug.h"
#include "GameFramework/CharacterMovementComponent.h"

ADemolisher::ADemolisher()
{
	GetCharacterMovement()->Mass = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

void ADemolisher::OnTriggerAttack(bool Start)
{
	bIsAttacking = Start;
	// Super::OnTriggerAttack(Start);
}

void ADemolisher::Throw()
{
	GameDebug::ShowDisplayLog(GetWorld(), "Throw");
}

void ADemolisher::Swing()
{
	GameDebug::ShowDisplayLog(GetWorld(), "Swing");
}

void ADemolisher::ChargeTo()
{
	GameDebug::ShowDisplayLog(GetWorld(), "ChargeTo");
}


