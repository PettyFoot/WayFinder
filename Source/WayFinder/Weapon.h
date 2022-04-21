// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class WAYFINDER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void UseItem(class AWayFinderCharacter* player) override;

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//Called from loot table to populate item with item info from enemies loot table
	//Sets item based on item info uobject (used for loot table item spawns)
	virtual void InitWithItemInfo(FItemInfoStruct iteminfo) override;


	
};
