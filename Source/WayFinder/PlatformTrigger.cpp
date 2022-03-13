// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MovingPlatform.h"
#include "WayFinderCharacter.h"

// Sets default values
APlatformTrigger::APlatformTrigger():
	ConnectedPlatformOne(nullptr),
	ConnectedPlatformTwo(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Trigger volume && begin and end overlap event delegates
	this->TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	if (this->TriggerVolume)
	{
		this->TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerVolumeOverlapBegin);
		this->TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerVolumeOverlapEnd);
	}

}

// Called when the game starts or when spawned
void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlatformTrigger::OnTriggerVolumeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto overlap_actor = Cast<AWayFinderCharacter>(OtherActor);
	if (overlap_actor)
	{
		if (this->ConnectedPlatformOne)
		{
			this->ConnectedPlatformOne->EnablePlatformMovement(true);
		}
		if (this->ConnectedPlatformTwo)
		{
			this->ConnectedPlatformTwo->EnablePlatformMovement(true);
		}

		//this->SetActorLocation((GetActorLocation() + FVector(0.f, 0.f, -15.f)));
	}
}

void APlatformTrigger::OnTriggerVolumeOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto overlap_actor = Cast<AWayFinderCharacter>(OtherActor);
	if (overlap_actor)
	{
		if (this->ConnectedPlatformOne)
		{
			this->ConnectedPlatformOne->EnablePlatformMovement(false);
		}
		if (this->ConnectedPlatformTwo)
		{
			this->ConnectedPlatformTwo->EnablePlatformMovement(false);
		}

		//this->SetActorLocation((GetActorLocation() + FVector(0.f, 0.f, 15.f)));
	}
}

// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

