// Fill out your copyright notice in the Description page of Project Settings.

#include "PathVector.h"

#include "PathFindingBoard.h"
#include "TraceChannelHelper.h"

FQuat UPathVector::NorthRotation = FQuat(FRotator(0.f, 180.f, 0.f));
FQuat UPathVector::EastRotation = FQuat(FRotator(0.f, 90.f, 0.f));
FQuat UPathVector::SouthRotation = FQuat(FRotator(0.f, 0.f, 0.f));
FQuat UPathVector::WestRotation = FQuat(FRotator(0.f, 270.f, 0.f));


UPathVector::UPathVector(FVector Position) : Location(Position) {}

