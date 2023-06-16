// Fill out your copyright notice in the Description page of Project Settings.

#include "SensorParent.h"
#include "../Adapter/ExportAdapter.h"

// Sets default values
ASensorParent::ASensorParent()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SensorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SensorMesh"));
  this->SensorMesh->SetupAttachment(this->RootComponent);

  AdapterComponent = CreateDefaultSubobject<UExportAdapter>(TEXT("ExportAdapter"));

}

// Called when the game starts or when spawned
void ASensorParent::BeginPlay()
{
  //If the TickInterval is below 0, the sensor is event-based
  if (this->TickInterval < 0.0f)
  {
    this->SetActorTickEnabled(false);
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bTickEvenWhenPaused = false;
  }
  //Time-based, based on the TickIntervall
  else
  {
    this->SetActorTickInterval(this->TickInterval);
  }
  Super::BeginPlay();
//  this->CreatePorts();
//  this->UpdatePorts();
  //Has to be done in case the SensorName changed
  this->Initialize();

  //Manage Output
  SensorOutput.TickInterval = this->TickInterval;
  SensorOutput.SensorName = this->SensorName;
  SensorOutput.SensorID = this->SensorID;
}

void ASensorParent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
//  this->DeletePorts();
}


// Called every frame
void ASensorParent::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  this->Sense(DeltaTime);
}

//void ASensorParent::CreatePorts()
//{
//  //Port for enabling and disabling the sensor
////  UE_LOG(LogTemp, Log, TEXT("SensorParent: Creating in_enable"));
////  this->in_enable = FinrocInterface::GetInstance()->CreateBoolReceive(SensorName + TEXT(" Enable"));
//
//}
//
//void ASensorParent::UpdatePorts()
//{
//  //FIXME this should not be necessary since the port was created with a name during the construction step
//  this->Enable->port_name = SensorName + TEXT(" Enable");
//  this->Enable->UpdatePortNames();
//
//}
//
//void ASensorParent::DeletePorts()
//{
//
////    if (in_enable)
////    {
////      UE_LOG(LogTemp, Log, TEXT("SensorParent: Deleting in_enable"));
////      delete in_enable;
////      in_enable = nullptr;
////    }
//}

void ASensorParent::UpdateParameters_Implementation()
{
  UE_LOG(LogTemp, Warning, TEXT("SensorParent UpdateParameters was called, this is probably a mistake and happens when there is no implementation of UpdateParameters_Implementation in the sensor."));
}

void ASensorParent::Sense_Implementation(const float &DeltaTime)
{
  UE_LOG(LogTemp, Warning, TEXT("SensorParent Sense was called, this is probably a mistake and happens when there is no implementation of Sense_Implementation in the sensor."));
}

void ASensorParent::PublishOutput()
{
  //Manage Output
  SensorOutput.TimestampUnreal = FDateTime::UtcNow();

}

bool ASensorParent::LoadPreset_Implementation(const FString& name)
{
  //TODO Log output
  return false;
}

void ASensorParent::UpdateActorName()
{
  //Rename(*SensorName);
  SetActorLabel(*SensorName);
}

void ASensorParent::Initialize()
{
  if (AdapterComponent)
  {
    AdapterComponent->SensorID = this->SensorID;
    AdapterComponent->SensorName = this->SensorName;
  }
}


