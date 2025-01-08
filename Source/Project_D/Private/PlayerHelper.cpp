// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHelper.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

FVector UPlayerHelper::MoveVectorUpward(const FVector& InVector, const float AddValue)
{
	return InVector + AddValue;
}

FVector UPlayerHelper::MoveVectorDownward(const FVector& InVector, const float SubValue)
{
	return InVector - SubValue;
}

FVector UPlayerHelper::MoveVectorForward(const FVector& InVector, const FRotator& InRotation, const float AddValue)
{
	return InVector + UKismetMathLibrary::GetForwardVector(InRotation) * AddValue;
}

FVector UPlayerHelper::MoveVectorBackward(const FVector& InVector, const FRotator& InRotation, const float SubValue)
{
	return InVector - UKismetMathLibrary::GetForwardVector(InRotation) * SubValue;
}

FVector UPlayerHelper::MoveVectorLeft(const FVector& InVector, const FRotator& InRotation, const float MoveValue)
{
	return InVector - UKismetMathLibrary::GetRightVector(InRotation) * MoveValue;
}

FVector UPlayerHelper::MoveVectorRight(const FVector& InVector, const FRotator& InRotation, const float MoveValue)
{
	return InVector + UKismetMathLibrary::GetRightVector(InRotation) * MoveValue;
}

FRotator UPlayerHelper::ReverseNormal(const FVector& InNormal)
{
	const FRotator B(0, 0, 100);
	return UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::MakeRotFromX(InNormal), B);
}