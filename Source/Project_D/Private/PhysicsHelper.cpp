// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsHelper.h"

float UPhysicsHelper::CalculateDuration(float Distance, float InitialVelocity, float Acceleration)
{
	// 예외 처리: 가속도가 0이면 단순 등속 운동 공식 사용
	if (FMath::IsNearlyZero(Acceleration))
	{
		if (FMath::IsNearlyZero(InitialVelocity))
		{
			return -1.0f; // 이동 불가
		}
		return Distance / InitialVelocity; // t = s / v
	}

	// 2차 방정식 형태: (1/2)at^2 + v0t - s = 0
	float a = 0.5f * Acceleration;
	float b = InitialVelocity;
	float c = -Distance;

	// 판별식 계산
	float Discriminant = FMath::Square(b) - 4 * a * c;
	if (Discriminant < 0)
	{
		return -1.0f; // 해 없음
	}

	// 근의 공식을 사용하여 시간 계산
	float t1 = (-b + FMath::Sqrt(Discriminant)) / (2 * a);
	float t2 = (-b - FMath::Sqrt(Discriminant)) / (2 * a);

	// 양의 시간 값을 선택
	float Time = FMath::Max(t1, t2);

	return Time;
}
