// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

class UCapsuleComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_D_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 
	virtual USkeletalMeshComponent* GetMesh() = 0;

	//
	virtual UCapsuleComponent* GetCapsule() = 0;
	
	/// Player 바닥 위치의 Z좌표값 반환
	/// @return Player의 발이 지면으로부터 Z축으로 얼마만큼 떨어져있는지 나타내는 값
	virtual float GetBottomZ() = 0;
};
