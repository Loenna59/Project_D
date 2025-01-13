// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleSystemComponent.h"

#include "PlayerHelper.h"
#include "PlayerInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UObstacleSystemComponent::UObstacleSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UObstacleSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
}

void UObstacleSystemComponent::Initialize()
{
	PlayerInterface = Cast<IPlayerInterface>(GetOwner());
	if (PlayerInterface)
	{
		PlayerMesh = PlayerInterface->GetMesh();
		PlayerCapsule = PlayerInterface->GetCapsule();
		PlayerMovement = PlayerInterface->GetCharacterMovement();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInterface is nullptr"));
	}
}


// Called every frame
void UObstacleSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const float Radius = PlayerCapsule->GetScaledCapsuleRadius();
	FVector StartEnd = PlayerCapsule->GetComponentLocation();
	StartEnd.Z = PlayerInterface->GetBottomZ();
	const TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartEnd,
		StartEnd,
		Radius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);
	bIsOnLand = true;
}

void UObstacleSystemComponent::TriggerInteractObstacle()
{
	bool bDetect;
	FVector HitLocation;
	FRotator ReverseNormal;
	DetectObstacle(bDetect, HitLocation, ReverseNormal, true);
	if (bDetect)
	{
		ScanObstacle(HitLocation, ReverseNormal, true);
		MeasureObstacle(true);
		TryInteractObstacle(true);
	}
}

void UObstacleSystemComponent::DetectObstacle(bool &bOutDetect, FVector &OutHitLocation, FRotator &OutReverseNormal, const bool &bVerbose) const
{
	const auto Owner = GetOwner();
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	FHitResult OutHit;

	for (int i = 0; i < 8; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector ActorLocation = Owner->GetActorLocation();
		const FRotator ActorRotation = Owner->GetActorRotation();
		const FVector TempVector = UPlayerHelper::MoveVectorUpward(
				UPlayerHelper::MoveVectorDownward(ActorLocation, 60.0f),
				i * 20
			);
		const FVector Start = UPlayerHelper::MoveVectorBackward(TempVector, ActorRotation, 30.0f);
		const FVector End = Owner->GetActorForwardVector() * 200.0f + TempVector;

		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			10.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		if (true == bHit)
		{
			break;
		}
	}
	
	bOutDetect = OutHit.bBlockingHit;
	OutHitLocation = OutHit.Location;
	OutReverseNormal = UPlayerHelper::ReverseNormal(OutHit.Normal);
}

void UObstacleSystemComponent::ScanObstacle(const FVector& DetectLocation, const FRotator& ReverseNormal, const bool& bVerbose)
{
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	FVector Start, End;
	bool bHit;
	
	// (300 - i * 10)만큼 위에서 LineTrace를 수행한다.
	// 널널하게 20만큼 뒤에서 시작하여 80만큼 앞에까지 선을 긋는다.
	for (int i = 0; i < 30; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector TempVector = UPlayerHelper::MoveVectorDownward(UPlayerHelper::MoveVectorUpward(DetectLocation, 300.0f), i * 10);
		Start = UPlayerHelper::MoveVectorBackward(TempVector, ReverseNormal, 20.0f);
		End = UPlayerHelper::MoveVectorForward(TempVector, ReverseNormal, 80.0f);
		bHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			Start,
			End,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerbose ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			FacedObstacleTopHitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		// LineTrace가 벽에 충돌하면
		if (true == bHit)
		{
			break;
		}
	}
	
	ObstacleRotation = UPlayerHelper::ReverseNormal(FacedObstacleTopHitResult.Normal);

	// FacedObstacleTopHitResult.Location을 기준으로 20씩 전진시켜가며 널널하게 위아래로 10만큼씩 범위로 하여 SphereTrace
	for (int i = 0; i < 10; i++)
	{
		const TArray<AActor*> ActorsToIgnore;
		const FVector A = UPlayerHelper::MoveVectorForward(FacedObstacleTopHitResult.Location, ObstacleRotation, i * 20);
		Start = UPlayerHelper::MoveVectorUpward(A, 10.0f);
		End = UPlayerHelper::MoveVectorDownward(Start, 10.0f);
		FHitResult OutHit;
		bHit = UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			5.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerbose ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			OutHit,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		if (0 == i)
		{
			if (true == bHit)
			{
				FirstTopHitResult = OutHit;
			}
		}
		else
		{
			if (true == bHit)
			{
				LastTopHitResult = OutHit;
			}
			else
			{
				break;
			}
		}
	}

	// LastTopHitResult는 장애물의 정확한 끝 지점이라고 볼 수 없다. (20씩 전진시켜가며 대강 측정한 것이기 때문)
	// LastTopHitResult 기준에서 20만큼 앞에 있는 위치를 시작으로 LastTopHitResult 까지 SphereTrace 하면 정확한 장애물의 끝 지점을 찾을 수 있다.
	Start = UPlayerHelper::MoveVectorForward(LastTopHitResult.ImpactPoint, ObstacleRotation, 20.0f);
	End = LastTopHitResult.ImpactPoint;
	const TArray<AActor*> ActorsToIgnore;
	bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start,
		End,
		10.0f,
		TraceChannel,
		false,
		ActorsToIgnore,
		bVerbose ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		EndOfObstacleHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	// 장애물의 정확한 끝 지점을 찾았다면 해당 지점부터 60만큼 앞에 있는 지점을 기준으로 시작하여 180만큼 아래에 있는 위치까지 SphereTrace
	if (true == bHit)
	{
		Start = UPlayerHelper::MoveVectorForward(EndOfObstacleHitResult.ImpactPoint, ObstacleRotation, 60.0f);
		End = UPlayerHelper::MoveVectorDownward(Start, 180.0f);
		UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			Start,
			End,
			10.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			bVerbose ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
			VaultLandingHitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.0f
		);

		if (true == bHit)
		{
			
		}
	}
}

void UObstacleSystemComponent::MeasureObstacle(const bool& bVerbose)
{
	// 만약 앞에 벽이 있는게 확실하다면
	if (FacedObstacleTopHitResult.bBlockingHit && FirstTopHitResult.bBlockingHit)
	{
		// Player 전방에 있는 벽의 높이 = 장애물 꼭대기 위치의 Z좌표 - Player의 발 위치 Z좌표
		ObstacleHeight = FirstTopHitResult.ImpactPoint.Z - PlayerInterface->GetBottomZ();
		
		if (bVerbose)
		{
			UE_LOG(LogTemp, Warning, TEXT("감지된 장애물의 높이 : %f"), ObstacleHeight);
		}
	}
	else
	{
		ObstacleHeight = 0.0f;
	}
}

void UObstacleSystemComponent::TryInteractObstacle(const bool& bVerbose)
{
	const AActor* Owner = GetOwner();
	const FVector Velocity = Owner->GetVelocity();
	const double SizeXY = UKismetMathLibrary::VSizeXY(Velocity);

	// 캐릭터의 수평속도가 5 이하라면 서 있는 상태라고 판단
	bool bIsStanding = UKismetMathLibrary::NearlyEqual_FloatFloat(SizeXY, 0.0f, 5.0f);
	
	if (FirstTopHitResult.bBlockingHit)
	{
		if (bIsOnLand)
		{
			// TODO: 각 행동에 필요한 높이 값을 에디터에서 수정할 수 있도록 UPROPERTY 세팅
			if (ObstacleHeight <= 300.0f)
			{
				if (bIsStanding)
				{
					// Mantle 동작 수행
					if (bVerbose) UE_LOG(LogTemp, Warning, TEXT("Mantle 동작 수행"));
				}
				else
				{
					// Vault 동작 수행
					if (ObstacleHeight > 100.0f)
					{
						// Front Flip 동작 수행
						if (bVerbose) UE_LOG(LogTemp, Warning, TEXT("Front Flip 동작 수행"));
						TryVault(EVaults::FrontFlip);
					}
					else if (ObstacleHeight > 90.0f)
					{
						// Two Hand Vault 동작 수행
						if (bVerbose) UE_LOG(LogTemp, Warning, TEXT("Two Hand Vault 동작 수행"));
						TryVault(EVaults::TwoHandVault);
					}
					else
					{
						// One Hand Vault 동작 수행
						if (bVerbose) UE_LOG(LogTemp, Warning, TEXT("One Hand Vault 동작 수행"));
						TryVault(EVaults::OneHandVault);
					}
				}
			}
			else
			{
				// 너무 높은 벽
				if (bVerbose) UE_LOG(LogTemp, Warning, TEXT("벽이 너무 높음"));
			}
		}
	}
}

void UObstacleSystemComponent::OnVaultMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bCanInteract = true;

	// Delegate 해제
	UAnimInstance* AnimInstance = PlayerMesh->GetAnimInstance();
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &UObstacleSystemComponent::OnVaultMontageEnded);
	AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UObstacleSystemComponent::OnVaultMontageBlendingOut);
}

void UObstacleSystemComponent::OnVaultMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	PlayerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlayerMovement->SetMovementMode(MOVE_Walking);
}

void UObstacleSystemComponent::TryVault(const EVaults VaultType)
{
	bCanInteract = false;
	
	// 장애물과의 상호작용 액션이 동작하는 중에 충돌 처리를 하지 않도록 함
	PlayerCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PlayerMovement->SetMovementMode(MOVE_Flying);

	UAnimInstance* AnimInstance = PlayerMesh->GetAnimInstance();
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &UObstacleSystemComponent::OnVaultMontageEnded);
	AnimInstance->OnMontageEnded.AddDynamic(this, &UObstacleSystemComponent::OnVaultMontageEnded);
	AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UObstacleSystemComponent::OnVaultMontageBlendingOut);
	AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UObstacleSystemComponent::OnVaultMontageBlendingOut);
	
	UAnimMontage* AnimMontage = nullptr;
	switch (VaultType)
	{
	case EVaults::OneHandVault:
		AnimMontage = OneHandVault;
		break;
	case EVaults::TwoHandVault:
		AnimMontage = TwoHandVault;
		break;
	case EVaults::FrontFlip:
		AnimMontage = FrontFlip;
		break;
	}
	
	AnimInstance->Montage_Play(AnimMontage);
}