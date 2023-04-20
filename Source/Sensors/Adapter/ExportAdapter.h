// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Adapter/AdapterComponent.h"
#include "ExportAdapter.generated.h"

/**
 *
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

  virtual void PublishLiDARData(struct FLiDARData LiDARStruct) override;

private:
  //Converts a row to a single FString
  FString CreateLine(const int32& LineNumber, const FVector& Location, const FColor& Color, const int32& ObjectID, const FString& Separator = TEXT(","));
};
