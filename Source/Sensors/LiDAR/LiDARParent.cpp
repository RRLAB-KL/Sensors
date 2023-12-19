// Fill out your copyright notice in the Description page of Project Settings.


#include "LiDAR/LiDARParent.h"




// Sets default values
ALiDARParent::ALiDARParent()
{
  // Set this actor to call Tick() every frame.
  PrimaryActorTick.bCanEverTick = true;

  SensorName = TEXT("LiDARActor");
  SensorID = 2;
}

void ALiDARParent::AddNoise_Implementation(FVector& Location, const float& RandomNoiseValue)
{
  UNoiseModels::AddVectorNoise(Location, RandomNoiseValue, Location);
}
