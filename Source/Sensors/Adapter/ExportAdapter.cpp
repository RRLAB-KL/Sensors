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

  if (this->FilePath.Len() == 0)
  {
    UE_LOG(LogTemp, Warning, TEXT("ExportAdapter No path for importing given."));
    return;
  }
  else if (FPaths::FileExists(this->FilePath))
  {
    TArray<FString> StringArray;
    //First line is needed for correct Unreal import
    StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate,RColor,GColor,BColor,ObjectID"));


    //TODO maybe parallel ?
    if (LiDARStruct.Locations.Num() != LiDARStruct.ObjectIDs.Num())
    {
      UE_LOG(LogTemp, Warning, TEXT("ExportAdapter Locations and ObjectIDs do not match in size. Locations array has %d many elements and the ObjectIDs array %D"), LiDARStruct.Locations.Num(), LiDARStruct.ObjectIDs.Num());
      return;
    }else if (LiDARStruct.Locations.Num() != LiDARStruct.RGBValues.Num()){
    	UE_LOG(LogTemp, Warning, TEXT("ExportAdapter Locations and RGBValues do not match in size. Locations array has %d many elements and the RGBValues array %D"), LiDARStruct.Locations.Num(), LiDARStruct.RGBValues.Num());
    	      return;
    }
    for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
    {
      StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i], LiDARStruct.RGBValues[i], LiDARStruct.ObjectIDs[i]));
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
