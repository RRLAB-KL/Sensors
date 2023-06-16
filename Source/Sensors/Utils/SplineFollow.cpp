// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/SplineFollow.h"


// Sets default values
ASplineFollow::ASplineFollow():
Start(false),
Timestep(0.01f),
CurrentTime(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	this->SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
  	this->SplineComponent->SetupAttachment(this->RootComponent);

}

// Called when the game starts or when spawned
void ASplineFollow::BeginPlay()
{
	Super::BeginPlay();

	//Since the sensor should be event-based, the TickIntervall of the sensor has to be < 0.0
	if(this->Sensor)
	{
		this->Sensor->TickInterval = -1.0f;
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("SplineFollow No sensor selected."));
	}
	
}

// Called every frame
void ASplineFollow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(this->Start && this->CurrentTime <= 1.0f)
	{
		FTransform NextTransform =  this->SplineComponent->GetTransformAtTime(this->CurrentTime, ESplineCoordinateSpace::Type::World, true, false);
		this->Sensor->SetActorTransform(NextTransform);

		this->CurrentTime += this->Timestep;
	}

}


void ASplineFollow::OnConstruction(const FTransform &Transform)
{
    Super::OnConstruction(Transform);

	for(auto& ele : this->CollisionBoxes)
	{
		if(ele)
		{
			ele->DestroyComponent();
		}
	}

	this->CollisionBoxes.Empty();

	for(size_t i = 1; i < this->SplineComponent->GetNumberOfSplinePoints()-1; i++)
	{
		UBoxComponent* CurrentBox = NewObject<UBoxComponent>(this);
		CurrentBox->OnComponentBeginOverlap.AddDynamic(this, &ASplineFollow::BeginOverlap);

		//Attach the DetectableActor to the current actor of OutActors
        FAttachmentTransformRules AttachementRule(EAttachmentRule::SnapToTarget, false);
		CurrentBox->AttachToComponent(this->RootComponent, AttachementRule);
		CurrentBox->SetVisibility(true,false);
		CurrentBox->SetHiddenInGame(false, false);
  		CurrentBox->SetCollisionProfileName("OverlapAll", true);
		CurrentBox->SetRelativeLocation(this->SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Type::Local));
		CurrentBox->SetGenerateOverlapEvents(true);

		this->CollisionBoxes.Emplace(CurrentBox);
	}
}


void ASplineFollow::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if(OtherActor->IsA(ASensorParent::StaticClass()))
	{
		ASensorParent* actor = Cast<ASensorParent>(OtherActor);
		actor->Sense(FApp::GetDeltaTime());
	}
}