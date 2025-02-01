// Fill out your copyright notice in the Description page of Project Settings.


#include "Demolisher.h"

#include "DemolisherAnimInstance.h"
#include "GameDebug.h"
#include "PlayerCharacter.h"
#include "TraceChannelHelper.h"
#include "VaultGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "FSM/DemolisherFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pathfinding/PathfindingComponent.h"
#include "Pathfinding/ZombieAIController.h"
#include "Project_D/Project_DCharacter.h"

ADemolisher::ADemolisher()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	AttackPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(GetMesh());
	
	AttackPoint->SetBoxExtent(FVector(50, 50, 50));
	AttackPoint->SetCollisionProfileName(TEXT("EnemyAttack"));
	AttackPoint->SetGenerateOverlapEvents(true);
	
	SetActiveAttackCollision(false);
}

void ADemolisher::BeginPlay()
{
	AActor::BeginPlay();

	CurrentHp = MaxHp;

	// GetMesh()->SetMassOverrideInKg(NAME_None, Mass);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	AnimationInstance = Cast<UDemolisherAnimInstance>(GetMesh()->GetAnimInstance());
	AI = Cast<AZombieAIController>(GetController());

	FSM = NewObject<UDemolisherFSMComponent>(this);
	AddOwnedComponent(FSM);
	FSM->RegisterComponent();

	FSM->ChangeState(EEnemyState::IDLE, this);

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABaseZombie::OnCollisionHit);

	if (AVaultGameModeBase* VaultGameModeBase = Cast<AVaultGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		VaultGameModeBase->IncreaseCount();
	}

	Pathfinding = NewObject<UPathfindingComponent>(this);
	AddOwnedComponent(Pathfinding);
	Pathfinding->RegisterComponent();

	Pathfinding->Initialize(this);

	if (GetMesh()->DoesSocketExist(TEXT("AttackSocket")))
	{
		AttackPoint->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("AttackSocket"));
	}
}

void ADemolisher::Tick(float DeltaSeconds)
{
	ACharacter::Tick(DeltaSeconds);

	if (CurrentHp <= 0)
	{
		return;
	}
	
	if (UWorld* const World = GetWorld())
	{
		if (bIsAttacking || bIsHitting)
		{
			return;
		}
		
		TraceChannelHelper::SphereTraceByChannel(
			World,
			this,
			GetActorLocation(),
			GetActorLocation(),
			FRotator::ZeroRotator,
			ECC_EngineTraceChannel2, // "Player"
			DetectRadius,
			true,
			false,
			[this] (bool bHit, TArray<FHitResult> HitResults)
			{
				bool HitPlayer = false;
				if (bHit)
				{
					for (FHitResult Hit : HitResults)
					{
						   AActor* HitActor = Hit.GetActor();
						   if (HitActor)
						   {
								if (HitActor->IsA<AProject_DCharacter>() || HitActor->IsA<APlayerCharacter>())
								{
									double Distance = FVector::Distance(HitActor->GetActorLocation(), GetActorLocation());
									
									AI->SetTarget(HitActor);
									
									if (FSM->GetCurrentState() == EEnemyState::WALK)
									{
										return;
									}

									int32 Rand = FMath::RandRange(1, 10);
									if (Rand > 3)
									{
										if (Distance <= MidRangeAttackRadius)
										{
											int32 Rand2 = FMath::RandRange(1, 10);
											if (Rand2 > 3)
											{
												if (Distance > AttackRadius)
												{
													FSM->ChangeState(EEnemyState::WALK, this);
												}
												else
												{
													// GameDebug::ShowDisplayLog(GetWorld(), FString::SanitizeFloat(Distance));
													FSM->ChangeState(EEnemyState::ATTACK, this);
												}	
											}
											else
											{
												FSM->ChangeState(EEnemyState::ATTACK, this);
											}
										}
										else
										{
											FSM->ChangeState(EEnemyState::WALK, this);
										}
									}
									else
									{
										FSM->ChangeState(EEnemyState::ATTACK, this);
									}
									HitPlayer = true;
									break;
								}
						   }
					}
				}

				if (!HitPlayer)
				{
					Evaluate();
					
				}
			}
		);
	}
}

void ADemolisher::SetActiveAttackCollision(bool Active)
{
	AttackPoint->SetVisibility(Active);
	AttackPoint->SetCollisionEnabled(Active? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ADemolisher::Throw()
{
	AI->StopMovement();
	bIsAttacking = true;

	FVector TargetLocation = AI->TargetActor->GetActorLocation();
	FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
	FRotator DestRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	SetActorRotation(DestRotation);
	
	AnimationInstance->PlayMontage(AI, AnimState::Throw, [](float _) {});
}

void ADemolisher::ChargeTo(float Speed, float Acceleration)
{
	AI->StopMovement();
	bIsAttacking = true;

	if (ChargingTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(ChargingTimerHandle);
		ChargingTimerHandle.Invalidate();
	}
	
    if (UDemolisherAnimInstance* D_Anim = Cast<UDemolisherAnimInstance>(AnimationInstance))
    {
        D_Anim->SetChargingAttack(true);
    }

    FVector TargetLocation = AI->TargetActor->GetActorLocation();
    FVector Location = GetActorLocation();
    FVector Direction = (TargetLocation - Location).GetSafeNormal();
    ChargeSpeed = Speed;

    GetWorldTimerManager().SetTimer(
        ChargingTimerHandle,
        [Direction, Acceleration, this]()
        {
        	if (!bIsAttacking)
        	{
        		if (ChargingTimerHandle.IsValid())
        		{
					GetWorldTimerManager().ClearTimer(ChargingTimerHandle);
					ChargingTimerHandle.Invalidate();
				}
        		return;
        	}
            GameDebug::ShowDisplayLog(GetWorld(), FString::SanitizeFloat(ChargeSpeed));
            ChargeSpeed += Acceleration;
            FVector Delta = Direction * ChargeSpeed * GetWorld()->GetDeltaSeconds();
            FVector Location = GetActorLocation();

        	FRotator DestRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
        	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DestRotation, 0.01f, 10.f);
        	
            SetActorLocationAndRotation(Location + Delta, NewRotation);

        	TraceChannelHelper::SphereTraceByChannel(
        		GetWorld(),
        		this,
        		Location,
        		Location + Delta,
        		FRotator::ZeroRotator,
        		ECC_EngineTraceChannel2,
        		100,
        		true,
        		true,
        		[this, Direction] (bool bHit, TArray<FHitResult> HitResults)
        		{
        			for (FHitResult Hit : HitResults)
                    {
        				UPrimitiveComponent* HitComp = Hit.GetComponent();
                        if (HitComp && HitComp->Mobility == EComponentMobility::Movable)
                        {
                            HitComp->AddImpulse(Direction * ChargeSpeed * 1000.f, NAME_None, true);
                        }
                    }
        		}
        	);
        },
        0.01f,
        true
    );
}

void ADemolisher::FinishAttack()
{
	if (ChargingTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(ChargingTimerHandle);
		ChargingTimerHandle.Invalidate();
	}

	if (UDemolisherAnimInstance* D_Anim = Cast<UDemolisherAnimInstance>(AnimationInstance))
	{
		D_Anim->SetChargingAttack(false);
	}
	
	Super::FinishAttack();
}
