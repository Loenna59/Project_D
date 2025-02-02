// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

template<typename T, typename... Args>
TArray<T> MakeArray(Args... _Args)
{
	return TArray<T>{_Args...};
}

class PROJECT_D_API TArrayHelper
{
public:
	TArrayHelper();
	~TArrayHelper();
};
