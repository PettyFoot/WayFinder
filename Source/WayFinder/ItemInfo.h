// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "Particles/ParticleSystem.h"
#include "ItemInfoEnums.h"
#include "ItemInfo.generated.h"



/**
 * 
 */


UCLASS()
class WAYFINDER_API UItemInfo : public UObject
{
	GENERATED_BODY()

public:
	UItemInfo();
};

USTRUCT(BlueprintType)
struct FConsumableType
{
	GENERATED_USTRUCT_BODY()

		//Set if this item should be a potion Consumable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties") //0
	EPotionType PotionType = EPotionType::PT_Default;

	//Set if this item should be a FoodDrink Consumable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties") //1
		EFoodDrinkType FoodDrink = EFoodDrinkType::FD_Default;

	//Set if this item should be a Buff consumable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties") //2
		EBuffType ConsumableBuffType = EBuffType::BUFF_Default;

	//-1 if not set
	int32 ConsumeableTypeIdx = -1;
	int32 GetConsumableTypeIdx()
	{
		//Will not set idx if more than one enum is set (default is not set)
		if (PotionType != EPotionType::PT_Default && ConsumeableTypeIdx == -1) { return ConsumeableTypeIdx = 0; }
		else if (FoodDrink != EFoodDrinkType::FD_Default && ConsumeableTypeIdx == -1) { return ConsumeableTypeIdx = 1; }
		else if (ConsumableBuffType != EBuffType::BUFF_Default && ConsumeableTypeIdx == -1) { return ConsumeableTypeIdx = 2; }
		else
		{
			 return ConsumeableTypeIdx = -1;
		}
	}

};


USTRUCT(BlueprintType)
struct FConsumableInfoStruct
{
	GENERATED_USTRUCT_BODY()

public:

	//Effect of consumable__deprecated needs to be update and removed
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties")
	EConsumableEffectType ConsumableEffectType;

	//struct to turn on or off certain effects of consumable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties")
	FConsumableType ConsumableType;

	//If this consumable is EOT(Effect over time)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties")
		bool bIsEOT;

	// Other (Only set for health consumes right now)
	//__ multiplier times item level: (baseeffectamount * itemlevel)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (ClampMin = 0.5f, ClampMax = 5.f))
		float BaseEffectAmount;

	//Amount to add to players damage
	//__ multiplier times item level: (enrageeffectamount * itemlevel) + baseweapondamage  = added damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (ClampMin = 0.5f, ClampMax = 3.f))
		float EnrageEffectAmount;

	//Amount to add to ult charge gain
	//__ smaller number is less ult charge rate buff
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (ClampMin = 2.f, ClampMax = 5.f))
		float InvigoreEffectAmount;

	//Amount to add to player's max health;
	// multiplier times item level: (fortifyeffectamount * itemlevel) + player max health
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (ClampMin = 3.f, ClampMax = 10.f))
		float FortifyEffectAmount;

	//Number of uses total
	//__1 by default means only 1 use of item anything more is multiple uses
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
		uint8 NumberOfUsesTotal;

	//Number of uses current/left
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true"))
		uint8 NumberOfUseLeft;

	//total effect time of consumable
	//__ seconds of effect time
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties", meta = (AllowPrivateAccess = "true", ClampMin = 1.f, ClampMax = 300.f))
	float EffectDuration;

};

USTRUCT(BlueprintType)
struct FWeaponType
{
	GENERATED_USTRUCT_BODY()

	//Set if this item is a ranged weapon
	//__Set to default if not using as ranged weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties")
	ERangedType RangedType = ERangedType::RT_Default;

	//Set if this item is a melee weapon
	//__Set to default if not using as melee weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Consumable Properties")
	EMeleeType MeleeType = EMeleeType::MT_Default;


};

USTRUCT (BlueprintType)
struct FWeaponInfoStruct
{
	GENERATED_USTRUCT_BODY()

public:

	//class of weapon (ranged, melee)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	EWeaponClass WeaponClass;

	//Type of weapon 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	FWeaponType WeaponType;

	//To set weapon based on weapon DT (Look at BaseMeleeWeapon header __ FWeaponStats)
	//TODO Add more rows in weapon level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
	EWeaponLevel WeaponLevel;

	//Base weapon damage 
	//__effected by WeaponLevel
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats", meta = (ClampMin = 1.0f))
	float BaseWeaponDamage;

	//Adjustment to damage if critical strike is landed 
	//__effected by WeaponLevel
	UPROPERTY(EditDefaultsOnly, Category = "Weapon FX", meta = (ClampMin = 1.5f, ClampMax = 5.f))
	float CriticalDamageAdjustment;

	//Starting durability of weapon
	//100 is default max
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats", meta = (ClampMin = 0.f, ClampMax = 100.f))
	float StartingWeaponDurability;

	//Starting ult charge rate of weapon
	//__effected by WeaponLevel
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats", meta = (ClampMin = 1.0f, ClampMax = 2.f))
	float UltChargeRate;

	//Rate of weapon durability loss per use
	//__effected by WeaponLevel
	// Starting durability * DurabilitLossRate = durability loss per use
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats", meta = (ClampMin = 0.0001f, ClampMax = 0.001f))
	float DurabilityLossRate;

		//Radius to apply ult effect within
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX", meta = (ClampMin = 1.0f, ClampMax = 10.f))
	float UltChargeRadius;

	//Dmaage of ult
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats", meta = (ClampMin = 1.0f, ClampMax = 20.f))
	float UltChargeDamage;


	//TODO
	UPROPERTY(EditDefaultsOnly, Category = "Weapon FX")
	UParticleSystem* DTImpactParticles;

	//TODO
	UPROPERTY(EditDefaultsOnly, Category = "Weapon FX")
	UParticleSystem* DTUltAbilityParticles;
		
};

class AItem;

USTRUCT(BlueprintType)
struct FItemInfoStruct
{
	GENERATED_USTRUCT_BODY()

public:

	//Static Mesh
	//__Root of all items
	//_____General Item Info:
	// There is a struct for the different item types (weapon, consumable, armor(TODO)) make sure to set the appropriate struct data based on item type
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UStaticMesh* ITemStaticMesh;

	//SkeletalMesh
	//__Use for weapon items
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		USkeletalMesh* ItemSkeletalMesh;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		//USkeletalMeshComponent* ItemSkeletalMeshComponent;

	//Item class (weapon, consumable, wearable, quest, readable)
	//__Helps initialize items on spawn 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		EItemClass ItemClass;

	//TODO__Not used anywhere
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AItem> ItemSubClass;

	//Name of Item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FString ItemDisplayName;

	//Item Description
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", MultiLine = "true"))
		FString ItemDescription;

	//Use action text
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FString UseActionText;

	//Thumbnail
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UTexture2D* ItemImage;

	//Inventory weight
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
		float ItemWeight;

	//true if item stackable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		bool bCanBeStacked;
	
	//Used in inventory to track how many of this items exist
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		uint8 ItemCurrentStack;

	//Max stack of item
	// __ 1 by default and if not stackable
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		uint8 ItemMaxStack = 1;
	
	//-1 if not inside an inventory (Default state!)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 InventorySlotIndex = -1;

	//Used to set certain aspects of the item's capabilities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 ItemLevel;

	//TODO__This is not used anywhere. May deprecate this
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		ELootTier LootTier;

	//Curve for item drop animation
	//__ X
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class UCurveFloat* SpawnDropCurve;

	//Curve for item drop animation
	//__ Y
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* SpawnDropCurve_Y;

	//Location of item drop __ helps assist animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
		FVector SpawnDropLocationTarget;

	//Size of pickup box area
	//__Use a test bp item and mesh to get size right
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	FVector PickupCollisionBoxExtent;

	//offset of pickup box area
	//__Use a test bp item and mesh to get size right
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	FVector PickupCollisionBoxOffset;

	//Weapon Item info struct
	//__Set stuff if item should be a weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FWeaponInfoStruct WeaponInfoStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FConsumableInfoStruct ConsumableInfoStruct;
};

USTRUCT(BlueprintType)
struct FItemDataBaseItemMap
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
		TMap<FString, FItemInfoStruct> LootA;

};

USTRUCT(BlueprintType)
struct FLootTableTierByLevel
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
		TArray<FItemDataBaseItemMap> LootTable_TierA;

	UPROPERTY(EditDefaultsOnly)
		TArray<FItemDataBaseItemMap> LootTable_TierB;

	UPROPERTY(EditDefaultsOnly)
		TArray<FItemDataBaseItemMap> LootTable_TierC;

	UPROPERTY(EditDefaultsOnly)
		TArray<FItemDataBaseItemMap> LootTable_TierD;

	UPROPERTY(EditDefaultsOnly)
		TArray<FItemDataBaseItemMap> LootTable_TierE;

};

UCLASS(BlueprintType)
class UItemDatabase : public UDataAsset
{
	GENERATED_BODY()


public:

	UPROPERTY(EditDefaultsOnly)
		TArray<FLootTableTierByLevel> LootTable_1_20_DT;

	UPROPERTY(EditDefaultsOnly)
	TArray<FLootTableTierByLevel> LootTable_21_30_DT;

	UPROPERTY(EditDefaultsOnly)
		TArray<FLootTableTierByLevel> LootTable_31_40_DT;

	UPROPERTY(EditDefaultsOnly)
		TArray<FLootTableTierByLevel> LootTable_41_50_DT;

	UPROPERTY(EditDefaultsOnly)
		TArray<FLootTableTierByLevel> LootTable_51_60_DT;
	
};

