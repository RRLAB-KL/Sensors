// Fill out your copyright notice in the Description page of Project Settings.


#include "ExportAdapter.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

// Sets default values for this component's properties
UExportAdapter::UExportAdapter()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

}

void UExportAdapter::PublishLiDARData(struct FLiDARData LiDARStruct)
{
  switch (this->OutputFormat)
  {
  case EOutputFormat::CSV_simple:
    if (this->FilePath.Len() == 0)
    {
      UE_LOG(LogTemp, Warning, TEXT("ExportAdapter No path for importing given."));
      return;
    }
    else if (FPaths::FileExists(this->FilePath))
    {
      TArray<FString> StringArray;

      //TODO maybe parallel ?
      //FIXME append first line depending on the things that should be published
      //only Locations
      if (LiDARStruct.Locations.Num() != LiDARStruct.ObjectIDs.Num() && LiDARStruct.Locations.Num() != LiDARStruct.RGBValues.Num())
      {
        StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate"));
        for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
        {
          StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i]));
        }
      }
      //Locations with RGB values and ObjectID
      else if (LiDARStruct.Locations.Num() == LiDARStruct.RGBValues.Num() && LiDARStruct.Locations.Num() == LiDARStruct.ObjectIDs.Num())
      {
        //First line is needed for correct Unreal import
        StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate,RColor,GColor,BColor,ObjectID"));

        for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
        {
          StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i], LiDARStruct.RGBValues[i], LiDARStruct.ObjectIDs[i]));
        }
      }
      //Locations with RGB
      else if(LiDARStruct.Locations.Num() == LiDARStruct.RGBValues.Num() && LiDARStruct.Locations.Num() != LiDARStruct.ObjectIDs.Num())
      {
    	  StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate,RColor,GColor,BColor"));
          for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
          {
            StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i], LiDARStruct.RGBValues[i]));
          }
	}
      else if (LiDARStruct.Locations.Num() != LiDARStruct.RGBValues.Num() && LiDARStruct.Locations.Num() == LiDARStruct.ObjectIDs.Num()) {
    	  StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate,BColor"));
          for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
          {
            StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i], LiDARStruct.ObjectIDs[i]));
          }
	}
      else
      {
        UE_LOG(LogTemp, Warning, TEXT("ExportAdapter Either only output the Locations or RGB values and ObjectsIDs. Locations array has %d many elements and the ObjectIDs array %d and RGB Values %d"), LiDARStruct.Locations.Num(), LiDARStruct.ObjectIDs.Num(), LiDARStruct.RGBValues.Num());
        return;
      }

      // We use the LoadFileToString to load the file into
      if (FFileHelper::SaveStringArrayToFile(StringArray, *FilePath))
      {
        UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Succesfully Written to the text file"));
      }
      else
      {
        UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Failed to write FString to file."));
      }
    }
    else
    {
      UE_LOG(LogTemp, Warning, TEXT("ExportAdapter File does not exist."));
    }

    break;
  default:
    break;
  }


}

FString UExportAdapter::CreateLine(const int32& LineNumber, const FVector& Location, const FColor& Color, const int32& ObjectID, const FString& Separator)
{
  FString Line;
  Line.Append(FString::FromInt(LineNumber));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.X));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.Y));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.Z));
  Line.Append(Separator);
  Line.Append(FString::FromInt(Color.R));
  Line.Append(Separator);
  Line.Append(FString::FromInt(Color.G));
  Line.Append(Separator);
  Line.Append(FString::FromInt(Color.B));
  Line.Append(Separator);
  Line.Append(FString::FromInt(ObjectID));

  return Line;
}

FString UExportAdapter::CreateLine(const int32& LineNumber, const FVector& Location, const FColor& Color, const FString& Separator)
{
  FString Line;
  Line.Append(FString::FromInt(LineNumber));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.X));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.Y));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.Z));
  Line.Append(Separator);
  Line.Append(FString::FromInt(Color.R));
  Line.Append(Separator);
  Line.Append(FString::FromInt(Color.G));
  Line.Append(Separator);
  Line.Append(FString::FromInt(Color.B));

  return Line;
}

FString UExportAdapter::CreateLine(const int32& LineNumber, const FVector& Location, const int32& ObjectID, const FString& Separator)
{
  FString Line;
  Line.Append(FString::FromInt(LineNumber));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.X));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.Y));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.Z));
  Line.Append(Separator);
  Line.Append(FString::FromInt(ObjectID));

  return Line;
}

FString UExportAdapter::CreateLine(const int32& LineNumber, const FVector& Location, const FString& Separator)
{
  FString Line;
  Line.Append(FString::FromInt(LineNumber));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.X));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.Y));
  Line.Append(Separator);
  Line.Append(FString::SanitizeFloat(Location.Z));
  return Line;
}

