// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <cmath>


#include <fstream>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/PointLightComponent.h"
#include "Engine/PointLight.h"
#include "Engine/SceneCapture2D.h"
#include "../Utils/SensorVisualization.h" //TODO weglassen für Übergabe
#include "../Utils/Utility.h"
#include "../General/SensorParent.h"

#include "RayTracingLiDARActor.generated.h"

/**
 * The sensor only creates pointcloud data if something was hit
 */

USTRUCT(BlueprintType)
struct FParRayTracingLiDARACtor
{
  GENERATED_BODY()

  /*number of channels for the lidar*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 ChannelCount;

  /*number of points per channel. Number of points per scan is this number muliplied by the channelcount*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 PointsToScanWithOneLaser;

  /*Red ligth is used for the intensity. Adjust for other intensity maps. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float IntensityOfRedLight;

  /*The horizontal FOV of the Lidar. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float HorizontalFov;

  /*The vertical FOV of the Lidar. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float VerticalFov;

  /*Which angle the middlemost beam has */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float VerticalFovOffset; //down [ +2 .. -24.8]

  /*Which horzontal angle the middlemost beam has */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float HorizontalFovOffset; //down [ +2 .. -24.8]

  /*the range of the lidar in meter*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float Range;

  /*Every point gets displaced in a random direction by this amount (in cm)*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float RandomNoiseValue;

  /*Drop Rays, which intensity is below a threshold*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool RaydroppingEnabled;

  /*show Lines with debug lines*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool visualizeRays;

  /*if true, every point gets the rgb information using scene capture components otherwise, the intensity value is returned in Intensity*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool UseRGBOutput;

  /*Flag for publishing the object ID, it is the first entry of the Tag Array*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool WriteObjectID;

  /*Flag for using Locations in global coordinates ot local coordinates based on the senros*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool UseGlobal;

  FParRayTracingLiDARACtor()
  {
    ChannelCount = 64;
    PointsToScanWithOneLaser = 2048;
    IntensityOfRedLight = 10000.0f;
    HorizontalFov = 360.0f;
    VerticalFov = 26.8f;
    VerticalFovOffset = 11.4f;
    HorizontalFovOffset = 0.0f;
    Range = 100.0f;
    RandomNoiseValue = 5.7;
    RaydroppingEnabled = true;
    visualizeRays = false;
    UseRGBOutput = false;
    WriteObjectID = false;
    UseGlobal = false;
  }

};

UCLASS()
class SENSORS_API ARayTracingLiDARActor : public ASensorParent
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  ARayTracingLiDARActor();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

private:

  void MakePointCloud();

  void ExportPointCloud();

  void CalculateProjectionMatrixMonoCams();

  bool ShootLaser(const float VerticalAngle, const float HorizontalAngle, FHitResult& HitResult, FCollisionQueryParams& TraceParams) const;

  void ReadRenderTarget(uint8_t index, TArray<FColor>& colorOut);

  void CalculateIntensity(TArray<FColor>(&Images)[3], TArray<FVector>& Locations, TArray<float>& Intensities);

  void AddHitLocations();

  void AddHitActors(std::map<AActor*, int>& Actors);

  /**
   * Assigns the an rgb value of the rendertargets to the corresponding point of the pointcloud.
   */
  void AssingRGB(TArray<FColor>(&Images)[3], TArray<FVector>& Locations, TArray<FColor>& RGBOutput);


public:



  //Read Only

  /*dividing Pointcloud to several simulation ticks for test purpose*/
  UPROPERTY(BlueprintReadOnly)
  uint8 SimulationSubsteps_ = 1;

  /*the current substep*/
  UPROPERTY(BlueprintReadOnly)
  uint8 currentSubstep_ = 0;

  /*Red light of the intensity*/
  UPROPERTY(VisibleAnywhere)
  class UPointLightComponent* RedLightComp;

  /*Sensor visualization class*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ASensorVisualization* SensorVisualization;

  /** Parameters for the RayTracingLiDARActor */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FParRayTracingLiDARACtor RayTracingLiDARActorParameters;

  /*Output struct of the sensor*/
  FLiDARData LiDAROutput;

  /**
   * This method adds Noise to the provided FVector in place. Overwrite for your own noise model.
   */
  UFUNCTION(BlueprintNativeEvent)
  void AddNoise(FVector& Location);

  virtual void AddNoise_Implementation(FVector& Location);

  virtual void PublishOutput() override;

  virtual void Sense_Implementation(const float &DeltaTime);


private:

  // variables
  std::vector<std::vector<FHitResult>> RecordedHits_;

  FTransform transform_;


  //derived from params
  float HorizontalAngleBetweenBeams_;
  std::vector<float> VerticalAngles_;
  // number of points = ChannelCount_  * PointsToScanWithOneLaser_


  //MONO CAM FOR INTENSITY
  ASceneCapture2D* sceneCapture_[3];
  UTextureRenderTarget2D* renderTarget_[3];
  FMatrix ProjectionMatrixMonoCam_[3];
  const float MonoCamHeight_ = 1024, MonoCamWidth_ = 1024;
  const float MonoCamFOV_ = 120; //dont change

  //misc:
  bool firstTick_ = true;
}; //end of class


//Inline Utilitiy functions

inline int clampInt(int value, int min, int max)
{
  return std::min(std::max(value, min), max);
}
