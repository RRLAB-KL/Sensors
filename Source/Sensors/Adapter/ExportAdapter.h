// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Adapter/AdapterComponent.h"
#include "ExportAdapter.generated.h"

UENUM(BlueprintType)
enum class EOutputFormat : uint8
{
  CSV_simple UMETA(DisplayName = "CSV"),
  Undefined UMETA(DisplayName = "Undefined")
};

UENUM(BlueprintType)
enum class EFileWriteDirective : uint8
{
  Replace,
  Append
};

/**
 * This adapter is used to export single sensor recordings (currently only LiDAR) to a csv file in a predefined format.
 */
UCLASS()
class SENSORS_API UExportAdapter : public UAdapterComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UExportAdapter();

public:
  /**Path to the file that should be used for writing the pointcloud*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString FilePath;

  /**Defines the output format of the data.*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EOutputFormat OutputFormat;

  /**Defines if the output should be appended to a file or should replace the current content.*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EFileWriteDirective FileWrie;

  virtual void PublishLiDARData(struct FLiDARData LiDARStruct) override;

private:
  //The current line number, used if the FileWriteDirective is Replace
  int32 current_line_number = 0;
  //FIXME Template?
  //Converts a row to a single FString
  FString CreateLine(const int32& LineNumber, const FVector& Location, const FColor& Color, const int32& ObjectID, const FString& Separator = TEXT(","));
  FString CreateLine(const int32& LineNumber, const FVector& Location, const FColor& Color, const FString& Separator = TEXT(","));
  FString CreateLine(const int32& LineNumber, const FVector& Location, const int32& ObjectID, const FString& Separator = TEXT(","));
  FString CreateLine(const int32& LineNumber, const FVector& Location, const FString& Separator = TEXT(","));
};
