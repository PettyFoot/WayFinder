// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Consumable.h"
#include "Food.generated.h"

/**
 * 
 */
UCLASS()
class WAYFINDER_API AFood : public AConsumable
{
	GENERATED_BODY()

public:

	AFood();

	virtual void UseItem(class AWayFinderCharacter* player) override;

	virtual void BeginPlay() override;
};
