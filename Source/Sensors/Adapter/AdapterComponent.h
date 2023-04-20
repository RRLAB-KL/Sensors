// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utils/Utility.h"
#include "AdapterComponent.generated.h"

/**
 * This component is an adapter for the communication between the RRLab sensors in Unreal and the world outside of Unreal, e.g. Finroc.
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SENSORS_API UAdapterComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UAdapterComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:

  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  //Publish Sensor data methods, should be overwritten by the specific adapter for the sensor structs needed

  virtual void PublishLiDARData(struct FLiDARData LiDARStruct);
  virtual void PublishIMUData(struct FIMUData IMUStruct);
  virtual void PublishGNSSData(struct FGNSSData GNSSStruct);

  /**Unique ID of the sensor this component is attached to*/
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int32 SensorID = -1;

  /**Name of the parent sensor*/
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FString SensorName;
};
