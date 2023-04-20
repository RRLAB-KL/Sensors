// Fill out your copyright notice in the Description page of Project Settings.

#include "RayTracingLiDARActor.h"
//#include "Exporter.h"

#include <set>
#include <cassert>
#include <iostream>
#include <typeinfo>

#include "Math/Vector.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "UnrealClient.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextureRenderTarget2D.h"


//HELPER
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring))
//printFString("test","test");

// Sets default values
ARayTracingLiDARActor::ARayTracingLiDARActor()
{
  // Set this actor to call Tick() every frame.
  PrimaryActorTick.bCanEverTick = true;

  SensorName = TEXT("RayTracingLiDARActor");
  SensorID = 2;
  this->Initialize();
}



// Called when the game starts or when spawned
void ARayTracingLiDARActor::BeginPlay()
{
  Super::BeginPlay();

  //UE_LOG(LogTemp, Warning, TEXT("Hello World!"));
  if (!this->RayTracingLiDARActorParameters.UseRGBOutput)
  {
    APointLight* lightActor = GetWorld()->SpawnActor<APointLight>(APointLight::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0));
    lightActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
    TArray<UActorComponent*> components;
    lightActor->GetComponents(components);
    RedLightComp = (UPointLightComponent*)components[0];
  }

  //RedLightComp = NewObject<UPointLightComponent>(this);
  if (RedLightComp)
  {

    RedLightComp->Mobility = EComponentMobility::Movable;
    //RedLightComp->SetRelativeLocation(FVector(0, 0, 0));
    RedLightComp->LightColor = FColor(255, 0, 0);
    RedLightComp->IntensityUnits = ELightUnits::Candelas;
    RedLightComp->Intensity = 0.0;
    RedLightComp->AttenuationRadius = this->RayTracingLiDARActorParameters.Range * 100.0f;

    RedLightComp->Activate();
    RedLightComp->InitializeComponent();
    RedLightComp->MarkRenderStateDirty();
    //RedLightComp->SetHiddenInGame(false, true);
  }


  float UpperFovLimit = this->RayTracingLiDARActorParameters.VerticalFov / 2.0f - this->RayTracingLiDARActorParameters.VerticalFovOffset;
  float AngleBetweenLines = this->RayTracingLiDARActorParameters.VerticalFov / this->RayTracingLiDARActorParameters.ChannelCount;

  //VerticalAngles
  for (int i = 0; i < this->RayTracingLiDARActorParameters.ChannelCount; i++)
  {
    VerticalAngles_.emplace_back(UpperFovLimit - i * AngleBetweenLines);
  }

  //make empty vectors in RecordedHits_
  RecordedHits_.resize(this->RayTracingLiDARActorParameters.ChannelCount);
  //HorizAngle Difference between Measure points
  HorizontalAngleBetweenBeams_ = 360.0f / this->RayTracingLiDARActorParameters.PointsToScanWithOneLaser;

  //No lens flares, no vignette, ...
  FEngineShowFlags noPostProcess = FEngineShowFlags(ESFIM_Game);
  noPostProcess.SetPostProcessing(false);

  //Mono Cameras for intensity
  for (int CamIdx = 0; CamIdx < 3; CamIdx++)
  {
    //TODO for Intensity values use only R channel
    //Create Render Component
    renderTarget_[CamIdx] = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), MonoCamWidth_, MonoCamHeight_, RTF_RGBA8, FLinearColor(FColor(0, 0, 0, 1)), false);
    renderTarget_[CamIdx]->bGPUSharedFlag = false;
    renderTarget_[CamIdx]->TargetGamma = 1.7;


    //Create Mono cams
    sceneCapture_[CamIdx] = (ASceneCapture2D*)GetWorld()->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass());
    sceneCapture_[CamIdx]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
    sceneCapture_[CamIdx]->AddActorLocalRotation(FRotator(0, MonoCamFOV_ * CamIdx, 0)); // 0, 120 and 240 Deg Cams

    auto* CaptureComponent = sceneCapture_[CamIdx]->GetCaptureComponent2D();
    CaptureComponent->bCaptureEveryFrame = false;
    CaptureComponent->bCaptureOnMovement = false;
    CaptureComponent->FOVAngle = MonoCamFOV_;
    CaptureComponent->CaptureSource = SCS_FinalColorLDR;
    CaptureComponent->TextureTarget = renderTarget_[CamIdx];
    CaptureComponent->ShowFlags = noPostProcess;

  }
  CalculateProjectionMatrixMonoCams();

  //UE_LOG(LogTemp, Warning, TEXT("END OF BEGIN PLAY!"));

}


// Called every frame
void ARayTracingLiDARActor::Tick(float DeltaTime)
{
  //UE_LOG(LogTemp, Warning, TEXT("TICK BEGIN!"));

  //execute Blueprint
  Super::Tick(DeltaTime);


  //clearing the results for each channel
  //also shooting images
  if (currentSubstep_ == 0) //Tick 0
  {
    if (!this->RayTracingLiDARActorParameters.UseRGBOutput)
    {
      //make 3 mono images, but with a red light
      RedLightComp->Intensity = this->RayTracingLiDARActorParameters.IntensityOfRedLight;
      RedLightComp->MarkRenderStateDirty();
    }
    sceneCapture_[0]->GetCaptureComponent2D()->CaptureScene();
    sceneCapture_[1]->GetCaptureComponent2D()->CaptureScene();
    sceneCapture_[2]->GetCaptureComponent2D()->CaptureScene();
    if (!this->RayTracingLiDARActorParameters.UseRGBOutput) //TODO not sure why
    {
      RedLightComp->Intensity = 0;
      RedLightComp->MarkRenderStateDirty();
    }

    RecordedHits_.resize(this->RayTracingLiDARActorParameters.ChannelCount);

    for (auto & hits : RecordedHits_)
    {
      hits.clear();
      hits.reserve(this->RayTracingLiDARActorParameters.PointsToScanWithOneLaser);
    }
  }

  //make the pointcloud (can be splitted into multiple steps )
  if (currentSubstep_ != SimulationSubsteps_) //Tick 0 until second-last
  {
    // shoots lasers and record hits (115 ms)
    MakePointCloud();

    transform_ = GetTransform();

    currentSubstep_++;
  }
  else //last Tick
  {
    //Export the point cloud, up to 400 ms(for read render target))
    ExportPointCloud();

    this->currentSubstep_ = 0;
  }

  //UE_LOG(LogTemp, Warning, TEXT("TICK END!"));
//TODO should only be called when something changes
  this->PublishOutput();
  this->AdapterComponent->PublishLiDARData(this->LiDAROutput);
}


//main function to make a point cloud [source:CARLA]
void ARayTracingLiDARActor::MakePointCloud()
{

  //  GetWorld()->GetPhysicsScene()->GetPxScene()->lockRead();
  //  {

  TRACE_CPUPROFILER_EVENT_SCOPE(ParallelFor);
  ParallelFor(this->RayTracingLiDARActorParameters.ChannelCount, [&](int32 idxChannel)
  {
    TRACE_CPUPROFILER_EVENT_SCOPE(ParallelForTask);

    FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Laser_Trace")), true/*, this */);
    TraceParams.bTraceComplex = true;
    TraceParams.bReturnPhysicalMaterial = false;
    TraceParams.bReturnFaceIndex = false;

    uint32 PointsToScan = this->RayTracingLiDARActorParameters.PointsToScanWithOneLaser / SimulationSubsteps_;
    //double HorizontalFovSubstep = this->RayTracingLiDARActorParameters.HorizontalFov/SimulationSubsteps_;
    for (auto idxPtsOneLaser = 0u; idxPtsOneLaser < PointsToScan; idxPtsOneLaser++)
    {
      FHitResult HitResult;
      const float VertAngle = VerticalAngles_[idxChannel];
      const float HorizAngle = std::fmod(this->RayTracingLiDARActorParameters.HorizontalFovOffset + HorizontalAngleBetweenBeams_
                                         * (currentSubstep_ * PointsToScan + idxPtsOneLaser), this->RayTracingLiDARActorParameters.HorizontalFov) - this->RayTracingLiDARActorParameters.HorizontalFov / 2.0f;

      if (ShootLaser(VertAngle, HorizAngle, HitResult, TraceParams))
      {
        RecordedHits_[idxChannel].emplace_back(HitResult);
      }
    };
  }); // parallel for

  // }  //lock scope
  //  GetWorld()->GetPhysicsScene()->GetPxScene()->unlockRead();
  //[end of source: CARLA]
}



//Calculates the intensity and exports the Point cloud
void ARayTracingLiDARActor::ExportPointCloud()
{
  //Get all hit Locations (9 ms + 9ms for noise)
  AddHitLocations();

  //for (auto loc : Locations)
  //{
  //  //DrawDebugPoint(GetWorld(), FVector(loc.X, loc.Y, loc.Z + 173), 2, FColor(255, 0, 0), true);
  //  //DrawDebugLine(GetWorld(), GetActorLocation(), FVector(loc.X, loc.Y, loc.Z + 173), FColor(255, 0, 0), true);
  //}

  //Preprocess Intensity (Render Target to Color Array)(400ms)
  TArray<FColor> ColorsOfImage[3];
  ReadRenderTarget(0, ColorsOfImage[0]);
  ReadRenderTarget(1, ColorsOfImage[1]);
  ReadRenderTarget(2, ColorsOfImage[2]);

  if (!this->RayTracingLiDARActorParameters.UseRGBOutput)
  {
    //Get the color of all points (8 ms)
    CalculateIntensity(ColorsOfImage, this->LiDAROutput.Locations, this->LiDAROutput.Intensities);
  }
  else
  {
    AssingRGB(ColorsOfImage, this->LiDAROutput.Locations, this->LiDAROutput.RGBValues);
  }

  //EXPORT
  //  Locations : Vector with all points (local coordinate system)
  //  Intensity: Intensity Values of points

  //DRAWDEBUG
  if (this->RayTracingLiDARActorParameters.visualizeRays || SensorVisualization != nullptr)
  {
    TArray<FVector> GlobalLocations;
    GlobalLocations.Reserve(this->LiDAROutput.Locations.Num());
    for (const auto & loc : this->LiDAROutput.Locations)
    {
      FVector GlobalLoc = transform_.TransformPosition(loc);
      if (this->RayTracingLiDARActorParameters.visualizeRays)
      {
        DrawDebugPoint(GetWorld(), FVector(GlobalLoc.X, GlobalLoc.Y, GlobalLoc.Z), 10, FColor(255, 0, 0), false, this->GetActorTickInterval());
        DrawDebugLine(GetWorld(), GetActorLocation(), FVector(GlobalLoc.X, GlobalLoc.Y, GlobalLoc.Z), FColor(255, 0, 0), false, this->GetActorTickInterval());
      }
      GlobalLocations.Emplace(GlobalLoc);
    }
    if (SensorVisualization != nullptr)
    {
      //Visualize the Locations using the SensorVisualization class
      SensorVisualization->VisualizePointcloud(GlobalLocations);
    }

  }
}



//Calculate the matrices for projecting 3D Points into the mono images
void ARayTracingLiDARActor::CalculateProjectionMatrixMonoCams()
{
  //Transform Matrix
  for (uint8_t i = 0; i < 3; i++)
  {
    const float sYaw = std::sin(FMath::DegreesToRadians(-MonoCamFOV_ * i));
    const float cYaw = std::cos(FMath::DegreesToRadians(-MonoCamFOV_ * i));

    const FMatrix Rotation = FMatrix(
                               FPlane(cYaw, sYaw, 0, 0),
                               FPlane(-sYaw, cYaw, 0, 0),
                               FPlane(0, 0, 1, 0),
                               FPlane(0, 0, 0, 1));

    const FPerspectiveMatrix projMatrix(FMath::DegreesToRadians(0.5f * MonoCamFOV_), MonoCamWidth_, MonoCamHeight_, GNearClippingPlane);

    const FMatrix CCS = FMatrix(
                          FPlane(0, 0, 1, 0),
                          FPlane(1, 0, 0, 0),
                          FPlane(0, 1, 0, 0),
                          FPlane(0, 0, 0, 1));

    ProjectionMatrixMonoCam_[i] = Rotation * CCS * projMatrix;
  }
}


//Shooting a single laser beam (concurrently executed!) [source: CARLA]
bool ARayTracingLiDARActor::ShootLaser(const float VerticalAngle, const float HorizontalAngle, FHitResult& HitResult, FCollisionQueryParams& TraceParams) const
{

  FHitResult HitInfo(ForceInit);

  FTransform ActorTransf = GetTransform();
  FVector LidarBodyLoc = ActorTransf.GetLocation();
  FRotator LidarBodyRot = ActorTransf.Rotator();

  FRotator LaserRot(VerticalAngle, HorizontalAngle, 0);  // float InPitch, float InYaw, float InRoll
  FRotator ResultRot = UKismetMathLibrary::ComposeRotators(
                         LaserRot,
                         LidarBodyRot
                       );
  //LidarBodyLoc += FVector(0.0f, 0.0f, -0.0f);


  FVector EndTrace = (this->RayTracingLiDARActorParameters.Range * 100.0) * UKismetMathLibrary::GetForwardVector(ResultRot) + LidarBodyLoc;

  GetWorld()->LineTraceSingleByChannel( // CARLA uses ParallelLineTraceSingleByChannel
    HitInfo,
    LidarBodyLoc,
    EndTrace,
    ECC_GameTraceChannel2,
    TraceParams,
    FCollisionResponseParams::DefaultResponseParam
  );

  if (HitInfo.bBlockingHit)
  {
    HitResult = HitInfo;
    return true;
  }
  else
  {
    return false;
  }
}


//preprocess mono image for intesity.
void ARayTracingLiDARActor::ReadRenderTarget(uint8_t index, TArray<FColor>& colorOut)
{

  //Image to Color Array
  colorOut.Empty(MonoCamHeight_ * MonoCamWidth_); //reserve 256 * 256

  FTextureRenderTargetResource* RenderResource = renderTarget_[index]->GameThread_GetRenderTargetResource();
  FReadSurfaceDataFlags ReadSurfaceDataFlags;
  ReadSurfaceDataFlags.SetLinearToGamma(false);

  RenderResource->ReadPixels(colorOut, ReadSurfaceDataFlags);
}


//Calculates the intensity for all points by projecting each point into the camera frame and lookup of the color
void ARayTracingLiDARActor::CalculateIntensity(TArray<FColor>(&Images)[3], TArray<FVector>& Locations, TArray<float>& Intensities)
{
  Intensities.Reserve(Locations.Num());
  Intensities.SetNum(Locations.Num());

  TRACE_CPUPROFILER_EVENT_SCOPE(ParallelFor); // 10ms instead of 30ms
  ParallelFor(Locations.Num(), [&](int32 i)
  {
    TRACE_CPUPROFILER_EVENT_SCOPE(ParallelForTask);
    FColor colorAtHitLocation;
    FVector& Location = Locations[i];

    int imageIdx = (Location.X * 1.73206 > Location.Y && Location.X * 1.73206 > -Location.Y) ? 0 : (Location.Y > 0) ? 1 : 2;

    FVector4 LocationHomogen = FVector4(Location.X, Location.Y, -Location.Z, 1);

    FVector4 LocationInImagePlane = ProjectionMatrixMonoCam_[imageIdx].TransformFVector4(LocationHomogen);

    int xCoord = (int)std::round((LocationInImagePlane[0] * MonoCamWidth_) / (2.0 * LocationInImagePlane[3]) + MonoCamWidth_ * 0.5);
    int yCoord = (int)std::round((LocationInImagePlane[1] * MonoCamHeight_) / (2.0 * LocationInImagePlane[3]) + MonoCamHeight_ * 0.5);

    xCoord = clampInt(xCoord, 0, MonoCamWidth_ - 1);
    yCoord = clampInt(yCoord, 0, MonoCamHeight_ - 1);

    int indexInArray = yCoord * MonoCamWidth_ + xCoord;
    Intensities[i] = float(Images[imageIdx][indexInArray].R) / 255.0;


  });
}

void ARayTracingLiDARActor::AssingRGB(TArray<FColor>(&Images)[3], TArray<FVector>& Locations, TArray<FColor>& RGBOutput)
{
  RGBOutput.Empty();
  RGBOutput.Reserve(Locations.Num());
  RGBOutput.SetNum(Locations.Num());

  TRACE_CPUPROFILER_EVENT_SCOPE(ParallelFor); // 10ms instead of 30ms
  ParallelFor(Locations.Num(), [&](int32 i)
  {
    TRACE_CPUPROFILER_EVENT_SCOPE(ParallelForTask);
    FColor colorAtHitLocation;
    FVector& Location = Locations[i];

    int imageIdx = (Location.X * 1.73206 > Location.Y && Location.X * 1.73206 > -Location.Y) ? 0 : (Location.Y > 0) ? 1 : 2;

    FVector4 LocationHomogen = FVector4(Location.X, Location.Y, -Location.Z, 1);

    FVector4 LocationInImagePlane = ProjectionMatrixMonoCam_[imageIdx].TransformFVector4(LocationHomogen);

    int xCoord = (int)std::round((LocationInImagePlane[0] * MonoCamWidth_) / (2.0 * LocationInImagePlane[3]) + MonoCamWidth_ * 0.5);
    int yCoord = (int)std::round((LocationInImagePlane[1] * MonoCamHeight_) / (2.0 * LocationInImagePlane[3]) + MonoCamHeight_ * 0.5);

    xCoord = clampInt(xCoord, 0, MonoCamWidth_ - 1);
    yCoord = clampInt(yCoord, 0, MonoCamHeight_ - 1);

    int indexInArray = yCoord * MonoCamWidth_ + xCoord;
    RGBOutput[i] = Images[imageIdx][indexInArray];
  });

}


//Adds all Linetracce hit locations into a single vector
void ARayTracingLiDARActor::AddHitLocations()
{
  this->LiDAROutput.Locations.Empty();
  this->LiDAROutput.Locations.Reserve(this->RayTracingLiDARActorParameters.PointsToScanWithOneLaser * this->RayTracingLiDARActorParameters.ChannelCount);
  this->LiDAROutput.ObjectIDs.Empty();
  this->LiDAROutput.ObjectIDs.Reserve(this->LiDAROutput.Locations.Num());


  for (std::vector<FHitResult>& channel : RecordedHits_)
  {
    for (FHitResult & hit : channel)
    {
      //printFString("test %s",*hit.ImpactPoint.ToString());
      //printFString("test %s",*hit.GetActor()->GetDebugName(hit.GetActor()));
      //
      //World Coordinates --> Local Coordinates
      FVector WorldPoint = hit.ImpactPoint;
      FVector LocalPoint = WorldPoint - this->GetActorLocation();

      //Add the noise to the locations
      this->AddNoise_Implementation(LocalPoint);
      this->LiDAROutput.Locations.Emplace(LocalPoint);

      //Manage the Object Id which is the first entry of the Tags Array in this case
      if (this->RayTracingLiDARActorParameters.WriteObjectID && hit.GetActor()!=nullptr && hit.GetActor()->Tags.Num() != 0)
      {
        FString ObjectID = hit.GetActor()->Tags[0].ToString();

        //if the first entry is no number, add -1
        if (ObjectID.IsNumeric())
        {
          this->LiDAROutput.ObjectIDs.Emplace(FCString::Atoi(*ObjectID));
        }
        else
        {
          this->LiDAROutput.ObjectIDs.Emplace(-1);
        }
      }
      else
      {
        this->LiDAROutput.ObjectIDs.Emplace(-1);
      }

      //////UV
      ////  //FVector2D UV;
      ////  //UGameplayStatics::FindCollisionUV(hit, 1, UV);
      ////  //UVs.emplace_back(UV);
    }
  }
}


//Adds all Linetracce hit Actors into a single map (second entry is number of occurrence of actor)
void ARayTracingLiDARActor::AddHitActors(std::map<AActor*, int>& Actors)
{

  for (std::vector<FHitResult>& channel : RecordedHits_)
  {
    for (FHitResult & hit : channel)
    {
      AActor* hitActor = hit.GetActor();
      if (hitActor)
      {
        if (Actors.count(hitActor) > 0)
          Actors[hitActor] = Actors[hitActor] + 1;
        else
          Actors[hitActor] = 1;
      }
    }
  }
}

void ARayTracingLiDARActor::PublishOutput()
{
  Super::PublishOutput();

  LiDAROutput.HorizontalFov = this->LiDAROutput.HorizontalFov;
  LiDAROutput.VerticalFov = this->RayTracingLiDARActorParameters.VerticalFov;
  LiDAROutput.HorizontalFovOffset = this->RayTracingLiDARActorParameters.HorizontalFovOffset;
  LiDAROutput.VerticalFovOffset = this->RayTracingLiDARActorParameters.VerticalFovOffset;
  LiDAROutput.HorizontalResolution = this->RayTracingLiDARActorParameters.HorizontalFov / this->RayTracingLiDARActorParameters.PointsToScanWithOneLaser;
  LiDAROutput.VerticalResolution = this->RayTracingLiDARActorParameters.VerticalFov / this->RayTracingLiDARActorParameters.ChannelCount;
  LiDAROutput.ChannelCount = this->RayTracingLiDARActorParameters.ChannelCount;
  LiDAROutput.PointsPerChannel = this->RayTracingLiDARActorParameters.PointsToScanWithOneLaser;
  LiDAROutput.Range = this->RayTracingLiDARActorParameters.Range;

}

void ARayTracingLiDARActor::AddNoise_Implementation(FVector& Location)
{
  UNoiseModels::AddVectorNoise(Location, this->RayTracingLiDARActorParameters.RandomNoiseValue, Location);
}

