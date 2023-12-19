// Fill out your copyright notice in the Description page of Project Settings.


#include "LiDAR/ParticleLiDAR.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"

// Sets default values
AParticleLiDAR::AParticleLiDAR()
{
  // Set this actor to call Tick() every frame.
  PrimaryActorTick.bCanEverTick = true;

  SensorName = TEXT("ParticleLiDAR");
  SensorID = 2;
  this->Initialize();
}


void AParticleLiDAR::PassParticleData(const TArray<FBasicParticleData>&  ParticleDataArray, const FVector Offset)
{

  Super::PublishOutput();

  if (ParticleDataArray.Num() != 0)
  {
    //prepare the output
    this->LiDAROutput.Locations.Reserve(ParticleDataArray.Num());
    this->LiDAROutput.Locations.SetNum(ParticleDataArray.Num());

    for (int32 Index = 0; Index != ParticleDataArray.Num(); ++Index)
    {
      this->LiDAROutput.Locations[Index] = ParticleDataArray[Index].Position + Offset;
      this->AddNoise(this->LiDAROutput.Locations[Index], this->ParticleLiDARParameters.RandomNoiseValue);
    }
  }
  if (this->AdapterComponent) {
      this->AdapterComponent->PublishLiDARData(this->LiDAROutput);
  }
  
}
