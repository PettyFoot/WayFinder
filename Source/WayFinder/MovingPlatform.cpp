// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"



// Sets default values
AMovingPlatform::AMovingPlatform():
	Speed(20.f),
	StartingTargetLocationOne(0.f),
	StartingTargetLocationTwo(0.f),
	StartingTargetLocationThree(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		this->InitReplication();

	}

	this->InitPlatformTargetLocations();
	
}

void AMovingPlatform::MovePlatform(float DeltaTime)
{
	FVector cur_loc = this->GetActorLocation();
	float journey_length = (this->StartingTargetLocationOne - this->StartingLocation).Size();
	float distance_traveled = (cur_loc - this->StartingLocation).Size();

	if (distance_traveled > journey_length)
	{
		FVector swap = this->StartingLocation;
		this->StartingLocation = this->StartingTargetLocationOne;
		this->StartingTargetLocationOne = swap;
	}

	FVector dir = (this->StartingTargetLocationOne - this->StartingLocation).GetSafeNormal();
	cur_loc += (this->Speed * DeltaTime * dir);
	SetActorLocation(cur_loc);
}

void AMovingPlatform::InitReplication()
{
	SetReplicates(true);
	SetReplicateMovement(true);
}

void AMovingPlatform::InitPlatformTargetLocations()
{

	this->StartingLocation = GetActorLocation();
	this->StartingTargetLocationOne = GetActorLocation() + this->TargetLocationOne;

	this->StartingTargetLocationTwo = GetActorLocation() + this->TargetLocationTwo;
	this->StartingTargetLocationThree = GetActorLocation() + this->TargetLocationThree;
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (this->bIsAbleToMove)
		{
			this->MovePlatform(DeltaTime);
		}
	}

}

