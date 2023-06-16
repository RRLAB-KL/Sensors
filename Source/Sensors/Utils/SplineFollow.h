// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../General/SensorParent.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "SplineFollow.generated.h"

UCLASS()
class SENSORS_API ASplineFollow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineFollow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Here, a trigger box is spawned for every spline point
	void OnConstruction(const FTransform &Transform) override;


public:

// Manage Overlap
  UFUNCTION()
  void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*The Spline where the sensor should be moved along. At every SplinePoint, the method Sense of the sensor is executed.*/
	UPROPERTY(VisibleAnywhere)
	USplineComponent* SplineComponent;

	/*The sensor that should be moved along the spline*/
	UPROPERTY(EditAnywhere)
	ASensorParent* Sensor;

	/*Starts the movement of the Sensor from the first point to the last calling the Sense method at every spline point*/
	UPROPERTY(EditAnywhere)
	bool Start;

	/*This value is added to the current time on the spline, should be normalized*/
	UPROPERTY(EditAnywhere)
	float Timestep;

private:

	/*Current time along the spline between 0.0 and 1.0*/
	float CurrentTime;

	/*There is a trigger box at every spline point for calling the sense function*/
	TArray<UBoxComponent*> CollisionBoxes;
	
};
