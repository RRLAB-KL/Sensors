// created by Manuel Vogel on 06.08.2022

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "LiDARParent.h"
#include "DepthCamLiDAR.generated.h"


USTRUCT(BlueprintType)
struct FParDepthCamLiDAR
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

  /*Every point gets displaced in a random direction by this amount (in cm)*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float RandomNoiseValue;

  FParDepthCamLiDAR()
  {
    ChannelCount = 32;
    PointsToScanWithOneLaser = 400;
    VerticalFov = 20.0f;
    VerticalFovOffset = 10.0f;
    RandomNoiseValue = 0.057f; // in m
  }

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SENSORS_API ADepthCamLiDAR : public ALiDARParent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  ADepthCamLiDAR();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  /* Materials for SceneCaptureCams, to transform Depth into 3D */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<UMaterial*> PostProcessMaterial;

  /* Parameters for the DepthCamLiDAR */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FParDepthCamLiDAR DepthCamLiDARParameters;

  /* Updates the TArray of Point Locations */
  UFUNCTION(BlueprintCallable)
  void MakePointCloud();

  virtual void PublishOutput() override;

private:
  FVector TransformPointToCameraFrame(const FMatrix& TransformMatrix, FVector point);

  ASceneCapture2D* sceneCapture_[3];
  UTextureRenderTarget2D* renderTarget_[3];

  FMatrix ProjectionMatrixCam_;
  float resolution = 1024;

};


