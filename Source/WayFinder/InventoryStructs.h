// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "InventoryStructs.generated.h"

/**
 * 
 */
class WAYFINDER_API InventoryStructs
{
public:
	InventoryStructs();
	~InventoryStructs();
};

class AItem;


USTRUCT(BlueprintType)
struct FIWeaponsMelee
{
	GENERATED_BODY()
		TArray<AItem*> Sword;
	TArray<AItem*> Axe;
	TArray<AItem*> Mace;
};
USTRUCT(BlueprintType)
struct FIWeaponsRanged
{
	GENERATED_BODY()
		TArray<AItem*> Bow;
	TArray<AItem*> Staff;
	TArray<AItem*> Hurlbat; // google it (anw axe to throw - i want it to throw like a boomerang)
};


USTRUCT(BlueprintType)
struct FIWeapons
{
	GENERATED_BODY()

	FIWeaponsMelee Melee;
	FIWeaponsRanged Ranged;

};


USTRUCT(BlueprintType)
struct FIWearablesHead
{
	GENERATED_BODY()
		TArray<AItem*> Light;
	TArray<AItem*> Medium;
	TArray<AItem*> Heavy;
};
USTRUCT(BlueprintType)
struct FIWearablesTorso
{
	GENERATED_BODY()
		TArray<AItem*> Light;
	TArray<AItem*> Medium;
	TArray<AItem*> Heavy;
};
USTRUCT(BlueprintType)
struct FIWearablesHands
{
	GENERATED_BODY()
		TArray<AItem*> Light;
	TArray<AItem*> Medium;
	TArray<AItem*> Heavy;
};
USTRUCT(BlueprintType)
struct FIWearablesLegs
{
	GENERATED_BODY()
		TArray<AItem*> Light;
	TArray<AItem*> Medium;
	TArray<AItem*> Heavy;
};
USTRUCT(BlueprintType)
struct FIWearablesFeet
{
	GENERATED_BODY()
		TArray<AItem*> Light;
	TArray<AItem*> Medium;
	TArray<AItem*> Heavy;
};

//TODO (Need to set up swappable mesh components at run time to do this and need armor class)
USTRUCT(BlueprintType)
struct FIWearables
{
	GENERATED_BODY()
	

	FIWearablesHead Head;
	FIWearablesTorso Torso;
	FIWearablesHands Hands;
	FIWearablesLegs Legs;
	FIWearablesFeet Feet;
};


USTRUCT(BlueprintType)
struct FIConsumablesPotions
{
	GENERATED_BODY()
	TArray<class AConsumable*> Health;
	TArray<AConsumable*> Ult; //TODO
	TArray<AConsumable*> Stamina; //TODO
	TArray<AConsumable*> Magica; //TODO _this isn't even a stat on the player yet
	TArray<AConsumable*> Poison; //TODO
	TArray<AConsumable*> Fire; //TODO
	TArray<AConsumable*> Bleed; //TODO
};
USTRUCT(BlueprintType)
struct FIConsumablesBuffs
{
	GENERATED_BODY()
		TArray<AConsumable*> Fortify;    //Increas max health
	TArray<AConsumable*> Enrage;     //Increase attack damage
	TArray<AConsumable*> Invigorate; //Increase ult charge rate
	TArray<AConsumable*> Swift;      //Increase movement speed
	TArray<AConsumable*> Shield;      //Increase shield

};


USTRUCT(BlueprintType)
struct FIConsumablesIngest
{
	GENERATED_BODY()
		TArray<AConsumable*> Food; //TODO
	TArray<AConsumable*> Drink; //TODO
};

USTRUCT(BlueprintType)
struct FIConsumables
{
	GENERATED_BODY()
	FIConsumablesPotions Potions;
	FIConsumablesBuffs Buffs;
	FIConsumablesIngest FoodDrink;

};
