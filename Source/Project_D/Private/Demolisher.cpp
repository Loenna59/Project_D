// Fill out your copyright notice in the Description page of Project Settings.


#include "Demolisher.h"

#include "DemolisherAnimInstance.h"
#include "GameDebug.h"
#include "PlayerCharacter.h"
#include "TraceChannelHelper.h"
#include "VaultGameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "FSM/DemolisherFSMComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pathfinding/PathfindingComponent.h"
#include "Pathfinding/ZombieAIController.h"
#include "Project_D/Project_DCharacter.h"

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
}

void ADemolisher::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

ADemolisher::ADemolisher()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

void ADemolisher::OnTriggerAttack(bool Start)
{
	bIsAttacking = Start;
}

void ADemolisher::Throw()
{
	// GameDebug::ShowDisplayLog(GetWorld(), "Throw");
	AnimationInstance->PlayMontage(AI, AnimState::Throw, [](float _){});
}

void ADemolisher::Swing()
{
	//GameDebug::ShowDisplayLog(GetWorld(), "Swing");
	AnimationInstance->PlayMontage(AI, AnimState::Swing, [](float _){});
}

void ADemolisher::ChargeTo(float Speed, float Acceleration)
{
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
			GameDebug::ShowDisplayLog(GetWorld(), FString::SanitizeFloat(ChargeSpeed));
			ChargeSpeed += Acceleration;
			FVector Delta = Direction * ChargeSpeed * 0.1f;
			FVector Location = GetActorLocation();
			
			SetActorLocation(Location + Delta);
		},
		0.1f,
		true
	);
}

void ADemolisher::FinishAttack()
{
	bIsWalkingDistance = -1;

	if (ChargingTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(ChargingTimerHandle);
		ChargingTimerHandle.Invalidate();
	}

	Super::FinishAttack();
}

void ADemolisher::Evaluate()
{
	Super::Evaluate();

	if (UDemolisherAnimInstance* D_Anim = Cast<UDemolisherAnimInstance>(AnimationInstance))
	{
		D_Anim->SetChargingAttack(false);
	}
}


