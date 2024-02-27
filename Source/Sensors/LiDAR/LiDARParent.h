// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "General/SensorParent.h"
#include "../Utils/SensorVisualization.h"
#include "../Utils/Utility.h"
#include "LiDARParent.generated.h"

/**
 * Parent Class for all Lidar Sensors
 */
UCLASS()
class SENSORS_API ALiDARParent : public ASensorParent
{
	GENERATED_BODY()

	public:
  // Sets default values for this component's properties
  ALiDARParent();

  /*Output struct of the sensor*/
  FLiDARData LiDAROutput;

  /*Sensor visualization class*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ASensorVisualization* SensorVisualization;

  /*Enables debug visualization of the sensor. The implementation depends on the specific sensors,*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool EnableDebugVisualization;

  /**
   * This method adds Noise to the provided FVector in place. Overwrite for your own noise model.
   */
  UFUNCTION(BlueprintNativeEvent)
  void AddNoise(FVector& Location, const float& RandomNoiseValue);

  virtual void AddNoise_Implementation(FVector& Location, const float& RandomNoiseValue);
};
