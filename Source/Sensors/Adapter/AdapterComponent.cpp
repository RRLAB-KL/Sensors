// Fill out your copyright notice in the Description page of Project Settings.


#include "AdapterComponent.h"


// Sets default values for this component's properties
UAdapterComponent::UAdapterComponent()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;
  // ...
}


// Called when the game starts
void UAdapterComponent::BeginPlay()
{
  Super::BeginPlay();

  // ...

}


// UAdapterComponent every frame
void UAdapterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ...
}

void UAdapterComponent::PublishLiDARData(struct FLiDARData LiDARStruct)
{
  UE_LOG(LogTemp, Warning, TEXT("AdapterComponent PublishLiDARData was called but is not implemented in the AdapterComponent."));
}
void UAdapterComponent::PublishIMUData(struct FIMUData IMUStruct)
{
  UE_LOG(LogTemp, Warning, TEXT("AdapterComponent PublishIMUData was called but is not implemented in the AdapterComponent."));
}
void UAdapterComponent::PublishGNSSData(struct FGNSSData GNSSStruct)
{
  UE_LOG(LogTemp, Warning, TEXT("AdapterComponent PublishGNSSData was called but is not implemented in the AdapterComponent."));
}

