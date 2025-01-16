// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableComponent.h"
#include "Zipline.generated.h"

UCLASS()
class PROJECT_D_API AZipline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZipline();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCableComponent* Cable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StartStick = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* EndStick = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* StartCablePosition = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* EndCablePosition = nullptr;
};
