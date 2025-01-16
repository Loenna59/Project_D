// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableComponent.h"
#include "Zipline.generated.h"

class UBoxComponent;

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
	UPROPERTY(EditAnywhere)
	USceneComponent* Root = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UCableComponent* Cable = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StartStick = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* EndStick = nullptr;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* StartCablePosition = nullptr;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* EndCablePosition = nullptr;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* StartStickColliderBox = nullptr;

	UFUNCTION()
    void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult);
    UFUNCTION()
    void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
