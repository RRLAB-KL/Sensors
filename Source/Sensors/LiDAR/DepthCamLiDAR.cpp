// created by Manuel Vogel on 06.08.2022

#include "DepthCamLiDAR.h"

#include "Kismet/KismetRenderingLibrary.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include <cmath>
#include "Materials/Material.h"
#include <string>

// Sets default values for this component's properties
ADepthCamLiDAR::ADepthCamLiDAR()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryActorTick.bCanEverTick = true;

  // Find post process materials to be applied to the depth cameras
  ConstructorHelpers::FObjectFinder<UMaterial> Loader(TEXT("Material'/Sensors/PostProcessing/M_PP_3D_1.M_PP_3D_1'"));
  //Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("Material'/Sensors/PostProcessing/M_PP_3D_1.M_PP_3D_1'")));
  if (Loader.Succeeded())
  {
    PostProcessMaterial.Add(Loader.Object);
  }
  else
  {
    UMaterial* Default = UMaterial::GetDefaultMaterial(MD_PostProcess);
    PostProcessMaterial.Add(Default);
  }
  ConstructorHelpers::FObjectFinder<UMaterial> Loader2(TEXT("Material'/Sensors/PostProcessing/M_PP_3D_2.M_PP_3D_2'"));
  if (Loader2.Succeeded())
  {
    PostProcessMaterial.Add(Loader2.Object);
  }
  ConstructorHelpers::FObjectFinder<UMaterial> Loader3(TEXT("Material'/Sensors/PostProcessing/M_PP_3D_3.M_PP_3D_3'"));
  if (Loader3.Succeeded())
  {
    PostProcessMaterial.Add(Loader3.Object);
  }
  /*UMaterial* Default = UMaterial::GetDefaultMaterial(MD_PostProcess);
  PostProcessMaterial.Add(Default);
  PostProcessMaterial.Add(Default);
  PostProcessMaterial.Add(Default);*/

  //USceneCaptureComponent2D* CaptureComponent1 = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("DepthLidar2"));
  //CaptureComponent1->Mobility = EComponentMobility::Movable;
  //CaptureComponent1->bVisualizeComponent = true;

  SensorName = TEXT("DepthCamLiDAR");
  SensorID = 2;
  this->Initialize();

}


// Called when the game starts
void ADepthCamLiDAR::BeginPlay()
{
  Super::BeginPlay();

  float FOV = 120;

  for (int CamIdx = 0; CamIdx < 3; CamIdx++)
  {
    //Create Render Component
    renderTarget_[CamIdx] = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), resolution, resolution, RTF_RGBA16f, FLinearColor(FColor(0, 0, 0, 1)), false);
    renderTarget_[CamIdx]->bGPUSharedFlag = false;
    renderTarget_[CamIdx]->TargetGamma = 1.7;


    //Create Mono cams
    sceneCapture_[CamIdx] = (ASceneCapture2D*)GetWorld()->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass());
    sceneCapture_[CamIdx]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
    sceneCapture_[CamIdx]->AddActorLocalRotation(FRotator(0, FOV * CamIdx, 0)); // 0, 120 and 240 Deg Cams

    auto* CaptureComponent = sceneCapture_[CamIdx]->GetCaptureComponent2D();
    CaptureComponent->bCaptureEveryFrame = false;
    CaptureComponent->bCaptureOnMovement = false;
    CaptureComponent->bAlwaysPersistRenderingState = true;
    CaptureComponent->FOVAngle = FOV;
    CaptureComponent->CaptureSource = SCS_FinalColorLDR;
    CaptureComponent->TextureTarget = renderTarget_[CamIdx];

    FWeightedBlendables PostProcessMats = TArray<FWeightedBlendable>
    { FWeightedBlendable(1.0, PostProcessMaterial[CamIdx]) };
    CaptureComponent->PostProcessSettings.WeightedBlendables = PostProcessMats;
  }

  //Calculate the matrix for projecting 3D Points into the images

  const FPerspectiveMatrix projMatrix(FMath::DegreesToRadians(0.5f * FOV), resolution, resolution, GNearClippingPlane);

  const FMatrix CCS = FMatrix(
                        FPlane(0, 0, 1, 0),
                        FPlane(1, 0, 0, 0),
                        FPlane(0, 1, 0, 0),
                        FPlane(0, 0, 0, 1));

  ProjectionMatrixCam_ = CCS * projMatrix;
}


// Called every frame
void ADepthCamLiDAR::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  this->MakePointCloud();
  if (SensorVisualization != nullptr)
  {
    //Visualize the Locations using the SensorVisualization class
    SensorVisualization->VisualizePointcloud(this->LiDAROutput.Locations);
  }
  this->AdapterComponent->PublishLiDARData(this->LiDAROutput);
  this->PublishOutput();

}


// main module to make a point cloud
void ADepthCamLiDAR::MakePointCloud()
{

  this->LiDAROutput.Locations.Empty();
  this->LiDAROutput.Locations.Reserve(this->DepthCamLiDARParameters.PointsToScanWithOneLaser * this->DepthCamLiDARParameters.ChannelCount);
  TArray<FFloat16Color> colorOut[3];

  //make depth images
  for (int i = 0; i < 3; i++)
  {
    sceneCapture_[i]->GetCaptureComponent2D()->CaptureScene();
    //UKismetRenderingLibrary::ExportRenderTarget(GetWorld(), renderTarget_[i], "C:\\Users\\vogel\\Desktop\\images", ("iamge"+ std::to_string(12) + ".png").c_str());
  }
  FTransform transform = this->GetActorTransform();

  //convert Images to Pixel Arrays
  for (int i = 0; i < 3; i++)
  {
    colorOut[i].Empty(resolution * resolution); //reserve space for pixels
    FTextureRenderTargetResource* RenderResource = renderTarget_[i]->GameThread_GetRenderTargetResource();
    //FReadSurfaceDataFlags ReadSurfaceDataFlags;
    //ReadSurfaceDataFlags.SetLinearToGamma(false);

    RenderResource->ReadFloat16Pixels(colorOut[i]/*, ReadSurfaceDataFlags*/);
  }

  //calculate Vertical and horizontal Angles

  std::vector<float> VerticalAngles, HorizontalAngles;

  //vertical
  float UpperFovLimit = this->DepthCamLiDARParameters.VerticalFov / 2.0f - this->DepthCamLiDARParameters.VerticalFovOffset;
  float VertAngleBetweenLines = this->DepthCamLiDARParameters.VerticalFov / static_cast<float>(this->DepthCamLiDARParameters.ChannelCount);

  for (int i = 0; i < this->DepthCamLiDARParameters.ChannelCount; i++)
  {
    VerticalAngles.emplace_back(UpperFovLimit - i * VertAngleBetweenLines);
  }

  float HorAngleBetweenLines = 360.0 / static_cast<float>(this->DepthCamLiDARParameters.PointsToScanWithOneLaser);

  for (float vertAngle : VerticalAngles)
  {
    for (int i = 0; i < static_cast<float>(this->DepthCamLiDARParameters.PointsToScanWithOneLaser) / 3; i++)
    {
      float horAngle = 60.0f - i * HorAngleBetweenLines;

      //project unit point on image
      FVector UnitPoint = UKismetMathLibrary::GetForwardVector(FRotator(vertAngle, horAngle, 0));

      //DrawDebugPoint(GetWorld(), transform.GetLocation()+ 1000* UnitPoint, 2, FColor(255, 0, 0), true);
      FVector UV = TransformPointToCameraFrame(ProjectionMatrixCam_, 100.0f * UnitPoint);
      if (UV.Z == 1) //valid
      {
        int index = int(UV[0]) + resolution * int(UV[1]);
        //int index = FMath::RandRange(0, int(resolution)-1) * resolution + FMath::RandRange(0, int(resolution)-1);

        FFloat16Color XYZI_1 = colorOut[0][index]; //in camera coordinates
        FFloat16Color XYZI_2 = colorOut[1][index]; //in camera coordinates
        FFloat16Color XYZI_3 = colorOut[2][index]; //in camera coordinates

        //in Unreal Coordinates System, but meters
        //(cam 0)

        //UE_LOG(LogTemp, Warning, TEXT("Point %f %f %f %f "), float(XYZI_1.B), float(XYZI_1.R), float(XYZI_1.G), float(XYZI_1.A));
        FVector XYZ_1 = FVector(
                          float(XYZI_1.B) - 500.0f,
                          float(XYZI_1.R) - 500.0f,
                          - float(XYZI_1.G) + 500.0f
                        );
        // (cam 1)
        FVector XYZ_2 = FVector(
                          float(XYZI_2.B) - 500.0f,
                          float(XYZI_2.R) - 500.0f,
                          - float(XYZI_2.G) + 500.0f
                        );
        // (cam 3)
        FVector XYZ_3 = FVector(
                          float(XYZI_3.B) - 500.0f,
                          float(XYZI_3.R) - 500.0f,
                          - float(XYZI_3.G) + 500.0f
                        ); //in Unreal Coordinates System, but meters


        //Add the noise to the locations
        this->AddNoise_Implementation(XYZ_1, this->DepthCamLiDARParameters.RandomNoiseValue);
        this->AddNoise_Implementation(XYZ_2, this->DepthCamLiDARParameters.RandomNoiseValue);
        this->AddNoise_Implementation(XYZ_3, this->DepthCamLiDARParameters.RandomNoiseValue);
        if (EnableDebugVisualization)
        {
          DrawDebugPoint(GetWorld(), transform.GetLocation() + 100 * XYZ_1, 2, FColor(255, 0, 0), false, this->TickInterval);
          DrawDebugPoint(GetWorld(), transform.GetLocation() + 100 * XYZ_2, 2, FColor(255, 0, 0), false, this->TickInterval);
          DrawDebugPoint(GetWorld(), transform.GetLocation() + 100 * XYZ_3, 2, FColor(255, 0, 0), false, this->TickInterval);
        }

        this->LiDAROutput.Locations.Add(XYZ_1 * 100.0f);
        this->LiDAROutput.Locations.Add(XYZ_2 * 100.0f);
        this->LiDAROutput.Locations.Add(XYZ_3 * 100.0f);
      }
    }
  }
}

//project single 3D point into 2D camera frame. Output is Vector in pixel coordinates
FVector ADepthCamLiDAR::TransformPointToCameraFrame(const FMatrix& TransformMatrix, FVector point)
{
  FVector pHomogen = FVector4(point.X, point.Y, -point.Z, 1); //UE Frame NEU to NED
  FVector4 point2D = TransformMatrix.TransformFVector4(pHomogen);

  // if Object is behind Camera --> point.w less than 1
  if (point2D[3] > 0.0)
  {

    //TODO: Do this a matrix Transform
    float x = (point2D[0] * resolution) / (2.0 * point2D[3]) + resolution * 0.5;
    float y = (point2D[1] * resolution) / (2.0 * point2D[3]) + resolution * 0.5;

    if (x > 0.0 && x < resolution && y > 0.0 && y < resolution)
    {
      //UE_LOG(LogTemp, Warning, TEXT("In Screen %f %f"), x, y);
      return FVector(x, y, 1);
    }
    else
    {
      //UE_LOG(LogTemp, Warning, TEXT("Not in Screen %f %f "), x, y);
      return FVector(x, y, 0);
    }
  }
  //UE_LOG(LogTemp, Warning, TEXT("Behind Screen %f %f "), 0, 0);
  return FVector(0, 0, 0);
}

void ADepthCamLiDAR::PublishOutput()
{
  Super::PublishOutput();

  LiDAROutput.HorizontalFov = 360.0f;
  LiDAROutput.VerticalFov = this->DepthCamLiDARParameters.VerticalFov;
  LiDAROutput.HorizontalFovOffset = 0.0f;
  LiDAROutput.VerticalFovOffset = this->DepthCamLiDARParameters.VerticalFovOffset;
  LiDAROutput.HorizontalResolution = LiDAROutput.HorizontalFov / this->DepthCamLiDARParameters.PointsToScanWithOneLaser;
  LiDAROutput.VerticalResolution = this->DepthCamLiDARParameters.VerticalFov / this->DepthCamLiDARParameters.ChannelCount;
  LiDAROutput.ChannelCount = this->DepthCamLiDARParameters.ChannelCount;
  LiDAROutput.PointsPerChannel = this->DepthCamLiDARParameters.PointsToScanWithOneLaser;
//  LiDAROutput.Range = this->DepthCamLiDARParameters.Range;

}
