// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LiDAR/LiDARParent.h"
#include "NiagaraDataInterfaceExport.h"
#include "ParticleLiDAR.generated.h"

/**Structure for parameters of the ParticleLiDAR*/
USTRUCT(BlueprintType)
struct FParParticleLiDAR
{
  GENERATED_BODY()

  /*number of channels for the lidar*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 ChannelCount;

  /*number of points per channel. Number of points per scan is this number muliplied by the channelcount*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 PointsToScanWithOneLaser;

  /*The vertical FOV of the Lidar. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float VerticalFov;

  /*Which angle the middlemost beam has */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float VerticalFovOffset;

  /*The horizontal FOV of the Lidar. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float HorizontalFov;

  /*Which horzontal angle the middlemost beam has */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float HorizontalFovOffset;

  /*Every point gets displaced in a random direction by this amount*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float RandomNoiseValue;

  FParParticleLiDAR()
  {
    ChannelCount = 64;
    PointsToScanWithOneLaser = 2048;
    VerticalFov = 32.0f;
    VerticalFovOffset = 0.0f;
    HorizontalFov = 360.0f;
    HorizontalFovOffset = 0.0f;
    RandomNoiseValue = 0.3f;
  }
};
/**
 * This class is the base class for particle based LiDAR sensors which are done using blueprints.
 * The main problem is that the output LiDAR struct is not exposed to Blueprints, restricting the access to c++ classes.
 */
UCLASS()
class SENSORS_API AParticleLiDAR : public ALiDARParent
{
  GENERATED_BODY()
public:
  // Sets default values for this actor's properties
  AParticleLiDAR();

  /** Parameters for the RayTracingLiDARActor */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FParParticleLiDAR ParticleLiDARParameters;

  /**Extracts the information from the particle data array to the LiDAR output struct, Replaces the PublishOutput method since it can not be accessed from Blueprints
   *
   * @param ParticleDataArray: The TArray of BasicParticleData
   * @param Offset: Location Offset of every point
   */
  UFUNCTION(BlueprintCallable)
  void PassParticleData(const TArray<FBasicParticleData>&  ParticleDataArray, const FVector Offset = FVector(0.0f, 0.0f, 0.0f));
};


