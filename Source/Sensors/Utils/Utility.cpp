#include "Utility.h"

void UNoiseModels::AddVectorNoise(FVector InVector, const float NoiseScaling, FVector& OutVector)
{
  OutVector = InVector + NoiseScaling * FMath::VRand();
}


void UNoiseModels::AddRotatorNoise(FRotator InRotator, FRotator& OutRotator, const float NoiseScaling, const float MinNoise, const float MaxNoise)
{
  if (MinNoise > MaxNoise)
  {
    UE_LOG(LogTemp, Warning, TEXT("UNoiseModels: MinNoise > MaxNoise should not be the case."));
    OutRotator = FRotator();
  }
  else if (MinNoise < -360.0f)
  {
    UE_LOG(LogTemp, Warning, TEXT("UNoiseModels: MinNoise should not be smaller than -360.0."));
    OutRotator = FRotator();
  }
  else if (MaxNoise > 360.0f)
  {
    UE_LOG(LogTemp, Warning, TEXT("UNoiseModels: MaxNoise should not be bigger than 360.0."));
    OutRotator = FRotator();
  }
  else
  {

    OutRotator = InRotator + FRotator(FMath::RandRange(NoiseScaling * MinNoise, NoiseScaling * MaxNoise), FMath::RandRange(NoiseScaling * MinNoise, NoiseScaling * MaxNoise), FMath::RandRange(NoiseScaling * MinNoise, NoiseScaling * MaxNoise));
  }
}
