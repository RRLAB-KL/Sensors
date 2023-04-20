// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Sensors.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(RRLabSensors);

class FSensors : public ISensors
{
  /** IModuleInterface implementation */
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FSensors, Sensors)



void FSensors::StartupModule()
{
  // This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FSensors::ShutdownModule()
{
  // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
  // we call this function before unloading the module.
}



