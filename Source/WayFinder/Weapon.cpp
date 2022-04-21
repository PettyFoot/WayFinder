// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "WayFinderCharacter.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AWeapon::UseItem(AWayFinderCharacter* player)
{

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeapon::InitWithItemInfo(FItemInfoStruct iteminfo)
{
	this->WeaponClass = iteminfo.WeaponInfoStruct.WeaponClass; //May need to move to AITEM parent call
	
	Super::InitWithItemInfo(iteminfo);
}

