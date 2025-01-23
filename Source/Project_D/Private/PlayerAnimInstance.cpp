// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "BaseZombie.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Player = Cast<APlayerCharacter>(GetOwningActor());
	if (Player)
	{
		Movement = Player->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		// Set velocity and ground speed from the movement components velocity. Ground speed is calculated from only the X and Y axis of the velocity, so moving up or down does not affect it.
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();

		// Set Should Move to true only if ground speed is above a small threshold (to prevent incredibly small velocities from triggering animations) and if there is currently acceleration (input) applied.
		bShouldMove = (GroundSpeed > 3.0f) && (UKismetMathLibrary::NotEqual_VectorVector(Movement->GetCurrentAcceleration(), FVector::ZeroVector, 0.0f));

		// Set Is Falling from the movement components falling state.
		bIsFalling = Movement->IsFalling();

		bIsDead = Player->bIsDead;
		
		PlayerState = Player->State;
		MovementVector = Player->MovementVector;
	}
}

void UPlayerAnimInstance::AnimNotify_OnDropkickImpact()
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimInstance::AnimNotify_OnDropkickImpact"));

	const FVector ActorLocation = Player->GetActorLocation(); // Actor의 월드 위치
	const FVector ForwardVector = Player->GetActorForwardVector(); // Actor의 전방 방향
	
	// 충격 설정
	const float Radius = 100.0f; // 드롭킥 범위
	const float ImpulseStrength = 100000.0f; // 얼마나 화끈하게 멀리 날려보내고 싶은가

	const FVector StartEnd = ActorLocation + (ForwardVector * 200.0f); // 전방으로 좀 전진한 곳에 드롭킥 범위 중심점 형성

	// 드롭킥 범위 안에 충돌체가 있는지 체크
	TArray<FHitResult> HitResults;
    const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);
    const bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        StartEnd,
        StartEnd,
        FQuat::Identity,
        ECC_GameTraceChannel3, // Enemy인데 이상하게 플레이어, 바닥까지 인식함. (IsA로 해결)
        CollisionShape
    );
    
    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
        	// 범위 안에 들어온 컴포넌트들..
	        if (UPrimitiveComponent* HitComponent = Hit.GetComponent())
        	{
	        	// 컴포넌트 소유 액터가 좀비일 경우에만...
	        	if (true == HitComponent->GetOwner()->IsA<ABaseZombie>())
	        	{
	        		HitComponent->SetSimulatePhysics(true); // 해당 컴포넌트의 물리 시뮬레이션을 활성화 하고
	        		const FVector Impulse = ForwardVector * ImpulseStrength; // 임펄스의 크기 벡터
	        		HitComponent->AddImpulseAtLocation(Impulse, Hit.ImpactPoint); // 날려버린다
	        	}
        	}
        }
    }

	// 범위 디버깅
	DrawDebugSphere(
		GetWorld(),
		StartEnd,
		Radius,
		12,
		FColor::Blue,
		false,
		2.0f
	);
}

void UPlayerAnimInstance::AnimNotify_OnStandingKickImpact()
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerAnimInstance::AnimNotify_OnStandingKickImpact"));
	
	const FVector ActorLocation = Player->GetActorLocation(); // Actor의 월드 위치
	const FVector ForwardVector = Player->GetActorForwardVector(); // Actor의 전방 방향
	
	// 충격 설정
	const float Radius = 100.0f; // 드롭킥 범위
	const float ImpulseStrength = 50000.0f; // 얼마나 화끈하게 멀리 날려보내고 싶은가

	const FVector StartEnd = ActorLocation + (ForwardVector * 200.0f); // 전방으로 좀 전진한 곳에 드롭킥 범위 중심점 형성

	// 드롭킥 범위 안에 충돌체가 있는지 체크
	TArray<FHitResult> HitResults;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);
	const bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		StartEnd,
		StartEnd,
		FQuat::Identity,
		ECC_GameTraceChannel3, // Enemy인데 이상하게 플레이어, 바닥까지 인식함. (IsA로 해결)
		CollisionShape
	);
    
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			// 범위 안에 들어온 컴포넌트들..
			if (UPrimitiveComponent* HitComponent = Hit.GetComponent())
			{
				// 컴포넌트 소유 액터가 좀비일 경우에만...
				if (true == HitComponent->GetOwner()->IsA<ABaseZombie>())
				{
					HitComponent->SetSimulatePhysics(true); // 해당 컴포넌트의 물리 시뮬레이션을 활성화 하고
					const FVector Impulse = ForwardVector * ImpulseStrength; // 임펄스의 크기 벡터
					HitComponent->AddImpulseAtLocation(Impulse, Hit.ImpactPoint); // 날려버린다
				}
			}
		}
	}

	// 범위 디버깅
	DrawDebugSphere(
		GetWorld(),
		StartEnd,
		Radius,
		12,
		FColor::Blue,
		false,
		2.0f
	);
}