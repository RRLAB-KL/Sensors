// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/DateTime.h"
//#include "GeoReferencingSystem.h"
#include "Utility.generated.h"

/**
 * Category Input refers to from outside
 *
 * Take care when displaying huge data to Unreal.
 */

/**
 * The different types of angles.
 * Degree Unsigned:  0 - 360
 * Degree Signed:  -180 - 180
 * Rad:
 */
UENUM(BlueprintType)
enum class EAngleType : uint8
{
  DEG_UNSIGNED UMETA(DisplayName = "Angle Degree Unsigned"),
  DEG_SIGNED UMETA(DisplayName = "Angle Degree Signed"),
  RAD_UNSIGNED UMETA(DisplayName = "Angle Rad Unsigned"),
  RAD_SIGNED UMETA(DisplayName = "Angle Rad Signed"),
};
struct FSensor
{

  /**The unique id of the sensor, defines which kind of specific sensor it is.*/
  int32 SensorID;

  /**
   * 0  SensorParent
   * 1  IMU Sensor
   * 2  LiDar Sensor
   * 3  GNSS
   */

  /**Bool that shows if the Sensor should be enabled or not, usually influences the events in the Tick event*/
  bool Enable = true;

  /**
   * from https://docs.unrealengine.com/5.0/en-US/API/Runtime/Core/Misc/FDateTime/:
   *
   * int32 GetMillisecond() : Gets this date's millisecond part (0 to 999).
   * int32 GetSecond()    : Gets this date's second part.
   * int64 GetTicks()   : Gets this date's representation as number of ticks.
   */
  /*The timestamp as FDateTime in Unreal, done since Unreal does not support double values*/
  FDateTime TimestampUnreal;

  /**The interval of the tick function, i.e. the update rate of the sensor*/
  float TickInterval;

  /**The name of the sensor*/
  FString SensorName;
};

USTRUCT(BlueprintType)
struct FCameraData
{
  GENERATED_BODY()

  /**/
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int SizeX;

  /**/
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int SizeY;

  //TODO maybe template
//    TArray<FColor> PixelBuffer;
//    TArray<FFloat16Color> PixelBufferFloat;

};


//USTRUCT(BlueprintType)
struct FIMUData
{
//  GENERATED_BODY()

  //TODO all in m/s
  //TODO koordinatensystem angeben

  /**Defines interpretation of the angle types*/
  EAngleType AngleType;

  /**Roll angle in degrees */
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float Roll;

  /**Pitch angle*/
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float Pitch;

  /**Yaw angle */
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float Yaw;

  /**Lateral force in centimeter per second*/
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float Sway;

  /**Longitudinal force in centimeter per second*/
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float Surge;

  /**Vertical force in centimeter per second*/
  //UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float Heave;

  /**angular velocity per second*/
  //UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float HeadingX;

  /**angular velocity per second*/
  //UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float HeadingY;

  /**angular velocity per second*/
  //UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float HeadingZ;

  //TODO methods for converting from deg to rad and maybe cm to m or flags

  //Converts all values above according to the selected AngleType
  void ConvertData()
  {
    switch (this->AngleType)
    {
    case EAngleType::DEG_SIGNED:
//TODO
      break;
    case EAngleType::RAD_UNSIGNED:
      UE_LOG(LogTemp, Warning, TEXT("The float value before is: %f"), this->Roll);
      this->Roll = FMath::DegreesToRadians(this->Roll);
      UE_LOG(LogTemp, Warning, TEXT("The float value after is: %f"), this->Roll);
      this->Pitch = FMath::DegreesToRadians(this->Pitch);
      this->Yaw = FMath::DegreesToRadians(this->Yaw);
      break;
    case EAngleType::RAD_SIGNED:
      //TODO
      break;
    default:
      break;
    }
  }

};

/**
 * Struct for the satellite data, used by the FGNSS struct
 */
struct FSatelliteData
{
  /**The id of the Satellite*/
  int32 ID;

  bool InUse;

  float Elevation;

  float Azimuth;

  float SignalStrength;

  FSatelliteData() {}

  FSatelliteData(const int32& id, const bool in_use, const float& elevation, const float&azimuth, const float& signal_strength)
  {
    this->ID = id;
    this->InUse = in_use;
    this->Elevation = elevation;
    this->Azimuth = azimuth;
    this->SignalStrength = signal_strength;
  }

};

//USTRUCT(BlueprintType)
struct FGNSSData
{
//  GENERATED_BODY()

  /** Current Actor Location */
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FVector ActorLocation;

  //TODO maybe quaternion?
  /** Current Actor Rotation in degree unsigned*/
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FRotator ActorRotation;

//  /*The projected coordinates in lat,long,alt*/
//  UPROPERTY(BlueprintReadOnly)
//  FGeographicCoordinates GeographicCoordinates;
//  /*The projected coordinates in Cartesian coordinates*/
//  UPROPERTY(BlueprintReadOnly)
//  FCartesianCoordinates CartesianCoordinates;
//
//  //TODO barely visible in the editor
//  /** The projected geographic GNSS location as FText, only for visualization in the editor since Unreal can not display double values*/
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//  FText GeographicLocation;
//
//  /** The projected Cartesian GNSS location as FText, only for visualization in the editor since Unreal can not display double values*/
//  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//  FText CartesianLocation;

  //Geographic coordinates
  //Latitude in degrees
  double        Latitude;
  //Longitude in degrees
  double        Longitude;
  //Altitude in meters
  double        Altitude;

  //TODO maybe easting northing up
  //Cartesian coordinates in meter
  double        X;
  double        Y;
  double        Z;

  float dGPSCorrectionAge; //http://lefebure.com/articles/rtk-correction-data-age-accuracy/
  double pdopNorth;                                  //!< PDOP in the north direction.
  double pdopEast;                                   //!< PDOP in the east direction.
  double pdopUp;                                     //!< PDOP in the up direction.

  /*Horizontal Dilution of Precision*/
//  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  float hdop;
  /*Vertical Dilution of Precision*/
//  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  float vdop;
  /*Position of Dilution of Precision*/
//  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  float pdop;

  //Satellite Data
  /**The maximum number of satellites*/
  int32 MaxSatelliteCount;

  /**The number of currently visible satellites*/
  int32 ActualVisibleSatelliteCount;

  /**The number of Satellites TODO*/
  int32 CountN;

  /**The number of Satellites TODO*/
  int32 CountS;

  /**The number of Satellites TODO*/
  int32 CountE;

  /**The number of Satellites TODO*/
  int32 CountW;

  /**For every visible satellite, store its satellite data*/
  TArray<FSatelliteData> SatelliteData;
};


struct FLiDARData
{
  /**
   * The local Locations of the points of the pointcloud, in meter.
   * For Iteration, see https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/TArrays/
   */
  TArray<FVector> Locations;

  /*The intensity values of the corresponding points of the Locations TArray*/
  TArray<float> Intensities;

  /*The RGB values of the corresponding points if they are rendered as normalized float value */
  TArray<FColor> RGBValues;

  /*The id of the object, it is defined by the first entry of the tags array of the hit object*/
  TArray<int32> ObjectIDs;

  /*The horizontal Field of View(FoV) of the sensor in degree*/
  float HorizontalFov;

  /*The horizontal Field of View(FoV) of the sensor in degree*/
  float VerticalFov;

  /*The horizontal Field of View(FoV) offset of the sensor in degree*/
  float HorizontalFovOffset;

  /*The vertical Field of View(FoV) offset of the sensor in degree*/
  float VerticalFovOffset;

  /*The horizontal resolution of the sensor in degree*/
  float HorizontalResolution;

  /*The vertical resolution of the sensor in degree*/
  float VerticalResolution;

  /*The number of channels in vertical direction*/
  int ChannelCount;

  /*The number of points per channel in horizontal direction*/
  int PointsPerChannel;

  /*The maximum range of the sensor in meter*/
  float Range;
};


UCLASS()
class SENSORS_API UNoiseModels : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:

  /*
   * Adds random noise to the vector by adding a scaled random unit vector to it.
   *
   * @param InVector The input vector with no noise
   * @param NoiseScaling The scaling factor for the random unit vector
   * @param OutVector The output vector, i.e. the input vector with noise
   */
  UFUNCTION(BlueprintCallable, Category = "NoiseModels")
  static void AddVectorNoise(FVector InVector, const float NoiseScaling, FVector& OutVector);



  /*
   * Adds rotational Noise on the InRotator depending on the NoiseScale and MinNoise and MaxNoise, if there is any problem, returns 0 rotator
   *
   *  @param InRotator Input rotation without noise.
   *  @param OutRotator Rotator with noise added.
   *  @param NoiseScaling Scaling factor of the random noise.
   *  @param MinNoise Minimum value for the noise value.
   *  @param MaxNoise Maximum value for the noise value
   */
  UFUNCTION(BlueprintCallable, Category = "NoiseModels")
  static void AddRotatorNoise(FRotator InRotator, FRotator& OutRotator, const float NoiseScaling = 1.0f, const float MinNoise = -360.0f, const float MaxNoise = 360.0f);

};

