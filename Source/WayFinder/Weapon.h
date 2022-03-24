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

	virtual void UseItem(class AWayFinderCharacter* player) override;

	virtual void BeginPlay() override;
	
};
