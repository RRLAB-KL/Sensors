// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Adapter/AdapterComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/Utility.h"
#include "Components/StaticMeshComponent.h"
#include "SensorParent.generated.h"

UCLASS()
class SENSORS_API ASensorParent : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  ASensorParent();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
  /**Bool that shows if the Sensor should be enabled or not, usually influences the events in the Tick event*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool Enable = true;

  /**The unique id of the sensor, defines which kind of specific sensor it is.*/
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int32 SensorID = 0;

  /** Name for Sensor*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString SensorName = TEXT("SensorParent");

  /** The interval of the tick function, i.e. the update rate of the sensor. 0.0f is as fast as possible.*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float TickInterval = 0.0f;

  /** Mesh of the Sensor */
  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* SensorMesh;

  /**Stores the struct of the output of every sensor */
  FSensor SensorOutput;

  /** Adapter of the sensor, used for communication between Unreal and the world outside, e.g Finroc */
  UPROPERTY(VisibleAnywhere)
  UAdapterComponent* AdapterComponent;

  // Called every frame
  virtual void Tick(float DeltaTime) override;

  /**Updates the name of the actor in the World Outliner using the SensorName*/
  UFUNCTION(BlueprintCallable)
  void UpdateActorName();

  /** Loads a preset from Name defined in the specific sensor class. */
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool LoadPreset(const FString& name);

  /**
   * This method is used to apply the changes of the parameters to the sensors, e.g. update the render target of a camera sensor
   *
   * Note that this is not the implementation, it just defines the name as well as the button for calling it in the editor.
   * The method that is overridden is the method UpdateParameters_Implementation
   */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, CallInEditor, Category = "Sensor Parent")
  void UpdateParameters();

  /**
   * The actual implementation of the above function.
   */
  virtual void UpdateParameters_Implementation();

  /**Initialization of the Sensor and the AdapterComponent, not called by SensorParent, as to be called by every Sensor individually since the variables are defined only after their specific constructors.
   */
  UFUNCTION(BlueprintCallable)
  void Initialize();

protected:
  /**This method is used to publish the output struct for the specific sensor depending on the target and the protocol.*/
  virtual void PublishOutput();
};
