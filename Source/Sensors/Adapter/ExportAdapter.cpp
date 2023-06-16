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
        if (this->current_line_number == 0)
        {
          StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate"));
        }
        for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
        {
          //Either use the global line number or the one of the current scan
          if (this->FileWrie == EFileWriteDirective::Append)
          {
            StringArray.Emplace(CreateLine(this->current_line_number, LiDARStruct.Locations[i]));
            this->current_line_number++;
          }
          else
          {
            StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i]));
          }
        }
      }
      //Locations with RGB values and ObjectID
      else if (LiDARStruct.Locations.Num() == LiDARStruct.RGBValues.Num() && LiDARStruct.Locations.Num() == LiDARStruct.ObjectIDs.Num())
      {
        if (this->current_line_number == 0)
        {
          //First line is needed for correct Unreal import
          StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate,RColor,GColor,BColor,ObjectID"));
        }

        for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
        {
          if (this->FileWrie == EFileWriteDirective::Append)
          {
            StringArray.Emplace(CreateLine(this->current_line_number, LiDARStruct.Locations[i], LiDARStruct.RGBValues[i], LiDARStruct.ObjectIDs[i]));
            this->current_line_number++;
          }
          else
          {
            StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i], LiDARStruct.RGBValues[i], LiDARStruct.ObjectIDs[i]));
          }
        }
      }
      //Locations with RGB
      else if (LiDARStruct.Locations.Num() == LiDARStruct.RGBValues.Num() && LiDARStruct.Locations.Num() != LiDARStruct.ObjectIDs.Num())
      {
        if (this->current_line_number == 0)
        {
          StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate,RColor,GColor,BColor"));
        }

        for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
        {
          if (this->FileWrie == EFileWriteDirective::Append)
          {
            StringArray.Emplace(CreateLine(this->current_line_number, LiDARStruct.Locations[i], LiDARStruct.RGBValues[i]));
            this->current_line_number++;
          }
          else
          {
            StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i], LiDARStruct.RGBValues[i]));
          }
        }
      }
      else if (LiDARStruct.Locations.Num() != LiDARStruct.RGBValues.Num() && LiDARStruct.Locations.Num() == LiDARStruct.ObjectIDs.Num())
      {
        if (this->current_line_number == 0)
        {
          StringArray.Emplace(TEXT("---,XCoordinate,YCoordinate,ZCoordinate,BColor"));
        }
        for (int32 i = 0; i < LiDARStruct.Locations.Num(); i++)
        {
          if (this->FileWrie == EFileWriteDirective::Append)
          {
            StringArray.Emplace(CreateLine(this->current_line_number, LiDARStruct.Locations[i], LiDARStruct.ObjectIDs[i]));
            this->current_line_number++;
          }
          else
          {
            StringArray.Emplace(CreateLine(i, LiDARStruct.Locations[i], LiDARStruct.ObjectIDs[i]));
          }
        }
      }
      else
      {
        UE_LOG(LogTemp, Warning, TEXT("ExportAdapter Either only output the Locations or RGB values and ObjectsIDs. Locations array has %d many elements and the ObjectIDs array %d and RGB Values %d"), LiDARStruct.Locations.Num(), LiDARStruct.ObjectIDs.Num(), LiDARStruct.RGBValues.Num());
        return;
      }

      /*For the write flags:
       *
       * enum EFileWrite
      *{
      *FILEWRITE_None                 = 0x00,
      *FILEWRITE_NoFail               = 0x01,
      *FILEWRITE_NoReplaceExisting    = 0x02,
      *FILEWRITE_EvenIfReadOnly       = 0x04,
      *FILEWRITE_Append               = 0x08,
      *FILEWRITE_AllowRead            = 0x10,
      *FILEWRITE_Silent               = 0x20,

      *}
       */
      bool write_success = false;
      if (this->FileWrie == EFileWriteDirective::Replace)
      {
        write_success = FFileHelper::SaveStringArrayToFile(StringArray, *FilePath);

      }
      else
      {
        //8 at the end for appending the string to the file
        write_success = FFileHelper::SaveStringArrayToFile(StringArray, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), 8);
      }
      if (write_success)
      {
        UE_LOG(LogTemp, Warning, TEXT("ExportAdapter: Successfully Written to the text file"));
      }
      else
      {
        UE_LOG(LogTemp, Warning, TEXT("ExportAdapter: Failed to write FString to file."));
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

