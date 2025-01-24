// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "BaseZombie.h"
#include "PlayerCharacter.h"
#include "ZombieTriggerParam.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Player = Cast<APlayerCharacter>(GetOwningActor());
	if (Player)
	{
		Movement = Player->GetCharacterMovement();
		ActionComponent = Player->ActionComponent;
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (nullptr == Player || nullptr == Movement)
	{
		// UE_LOG(LogTemp, Error, TEXT("UPlayerAnimInstance::NativeUpdateAnimation: nullptr"));
		return;
	}
	
	// Movements
	Velocity = Movement->Velocity;
	GroundSpeed = Velocity.Size2D();
	bShouldMove = (GroundSpeed > 3.0f) && (UKismetMathLibrary::NotEqual_VectorVector(Movement->GetCurrentAcceleration(), FVector::ZeroVector, 0.0f));
	bIsFalling = Movement->IsFalling();
	MovementVector = Player->MovementVector;

	// States
	bIsDead = Player->bIsDead;
	PlayerState = Player->State;
}

void UPlayerAnimInstance::AnimNotify_OnDropkickImpact()
{
	UE_LOG(LogTemp, Display, TEXT("UPlayerAnimInstance::AnimNotify_OnDropkickImpact"));
	if (nullptr == Player || nullptr == Movement || nullptr == ActionComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerAnimInstance::AnimNotify_OnDropkickImpact: nullptr"));
		return;
	}
	
	const FVector ActorLocation = Player->GetActorLocation(); // Actor의 월드 위치
	const FVector ForwardVector = Player->GetActorForwardVector(); // Actor의 전방 방향

	// 드롭킥 범위
	const float Radius = Player->ActionComponent->DropkickRadius;
	// 얼마나 화끈하게 멀리 날려보내고 싶은가
	const float ImpulseStrength = Player->ActionComponent->DropkickImpulseStrength;
	// 전방으로 좀 전진한 곳에 드롭킥 범위 중심점 형성
	const FVector StartEnd = ActorLocation + (ForwardVector * Player->ActionComponent->DropkickForwardOffset);

	// 드롭킥 범위 안에 충돌체가 있는지 체크
	TArray<FHitResult> HitResults;
    const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);
    const bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        StartEnd,
        StartEnd,
        FQuat::Identity,
        ECC_GameTraceChannel3,
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
	        	if (ABaseZombie* Zombie = Cast<ABaseZombie>(HitComponent->GetOwner()))
	        	{
	        		HitComponent->SetSimulatePhysics(true); // 해당 컴포넌트의 물리 시뮬레이션을 활성화 하고
	        		const FVector Impulse = ForwardVector * ImpulseStrength; // 임펄스의 크기 벡터
	        		HitComponent->AddImpulseAtLocation(Impulse, Hit.ImpactPoint); // 벡터 방향으로 날려버린다
	        		AZombieTriggerParam* Param = NewObject<AZombieTriggerParam>();

	        		// 데미지 처리
	        		Param->Damage = 99999;
	        		Param->HitResult = Hit;
	        		Zombie->OnTriggerEnter(Player, Param);
	        	}
        	}
        }
    }

	if (true == ActionComponent->bVerboseDropkick)
	{
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
}

void UPlayerAnimInstance::AnimNotify_OnStandingKickImpact()
{
	UE_LOG(LogTemp, Display, TEXT("UPlayerAnimInstance::AnimNotify_OnStandingKickImpact"));
	if (nullptr == Player || nullptr == Movement || nullptr == ActionComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerAnimInstance::AnimNotify_OnStandingKickImpact: nullptr"));
		return;
	}
	
	const FVector ActorLocation = Player->GetActorLocation(); // Actor의 월드 위치
	const FVector ForwardVector = Player->GetActorForwardVector(); // Actor의 전방 방향
	
	// 충격 설정
	const float Radius = Player->ActionComponent->StandingKickRadius; // 드롭킥 범위
	const float ImpulseStrength = Player->ActionComponent->StandingKickImpulseStrength; // 얼마나 화끈하게 멀리 날려보내고 싶은가
	const FVector StartEnd = ActorLocation + (ForwardVector * Player->ActionComponent->StandingKickForwardOffset); // 전방으로 좀 전진한 곳에 드롭킥 범위 중심점 형성

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
				if (ABaseZombie* Zombie = Cast<ABaseZombie>(HitComponent->GetOwner()))
				{
					HitComponent->SetSimulatePhysics(true); // 해당 컴포넌트의 물리 시뮬레이션을 활성화 하고
					const FVector Impulse = ForwardVector * ImpulseStrength; // 임펄스의 크기 벡터
					HitComponent->AddImpulseAtLocation(Impulse, Hit.ImpactPoint); // 날려버린다
					AZombieTriggerParam* Param = NewObject<AZombieTriggerParam>();
					Param->Damage = 99999;
					Param->HitResult = Hit;
					Zombie->OnTriggerEnter(Player, Param);
				}
			}
		}
	}

	if (true == ActionComponent->bVerboseStandingKick)
	{
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
}