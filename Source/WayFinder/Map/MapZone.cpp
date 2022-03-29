// Fill out your copyright notice in the Description page of Project Settings.


#include "MapZone.h"
#include "Components/BoxComponent.h"

// Sets default values
AMapZone::AMapZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->ZoneOne = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneOne"));
	this->ZoneTwo = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneTwo"));
	this->ZoneThree = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneThree"));
	this->ZoneFour = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneFour"));

}

// Called when the game starts or when spawned
void AMapZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMapZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

