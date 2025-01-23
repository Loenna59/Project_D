// Fill out your copyright notice in the Description page of Project Settings.


#include "Demolisher.h"

#include "GameFramework/CharacterMovementComponent.h"

ADemolisher::ADemolisher()
{
	GetCharacterMovement()->Mass = 300.f;
	GetCharacterMovement()->MaxWalkSpeed = 100.f;
}
