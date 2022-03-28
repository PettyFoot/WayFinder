// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"

#include "WayFinderCharacter.h"


AFood::AFood() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFood::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AFood::UseItem(AWayFinderCharacter* player)
{

}

void AFood::BeginPlay()
{
	Super::BeginPlay();
}

void AFood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
