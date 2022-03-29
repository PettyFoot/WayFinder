// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "Particles/ParticleSystem.h"
#include "ItemInfo.generated.h"



/**
 * 
 */



UENUM(BlueprintType)
enum class EItemClass : uint8
{
	IC_Weapon UMETA(DisplayName = "Weapon"),
	IC_Consumable UMETA(DisplayName = "Consumable"),
	IC_Armor UMETA(DisplayName = "Armor"),
	IC_QuestItem UMETA(DisplayName = "Quest Item"),
	IC_Readable UMETA(DisplayName = "Readable"),
	IC_Default  UMETA(DisplayName = "Default")

};


//Used to seperate item info within the datasset list
UENUM(BlueprintType)
enum class ELootTableLevelBounds : uint8
{
	ELTLB_One_Twenty UMETA(DisplayName = "1-20"),
	ELTLB_TwentyOne_Thirty UMETA(DisplayName = "21-30"),
	ELTLB_ThirtyOne_Fourty UMETA(DisplayName = "31-40"),
	ELTLB_FourtyOne_Fifty UMETA(DisplayName = "41-50"),
	ELTLB_FiftyOne_Sixty UMETA(DisplayName = "51-60"),
	ELTLB_Default UMETA(DisplayName = "Default")

};
UENUM(BlueprintType)
enum class ELootTier : uint8
{
	ELTLB_TierA UMETA(DisplayName = "Tier A"),
	ELTLB_TierB UMETA(DisplayName = "Tier B"),
	ELTLB_TierC UMETA(DisplayName = "Tier C"),
	ELTLB_TierD UMETA(DisplayName = "Tier D"),
	ELTLB_TierE UMETA(DisplayName = "Tier E"),
	ELTLB_TierDefault UMETA(DisplayName = "Tier Default")
	

};

UENUM(BlueprintType)
enum class EWeaponLevel : uint8
{
	WL_NoviceWeapon UMETA(DisplayName = "Novice"),
	WL_ApprenticeWeapon UMETA(DisplayName = "Apprentice"),
	WL_AdeptWeapon UMETA(DisplayName = "Adept"),
	WL_MasterWeapon UMETA(DisplayName = "Master"),
	WL_ExhaltedWeapon UMETA(DisplayName = "Exhalted"),
	WL_LegendaryWeapon UMETA(DisplayName = "Legendary"),
	WL_DefaultWeapon UMETA(DisplayName = "Default")

};


UCLASS()
class WAYFINDER_API UItemInfo : public UObject
{
	GENERATED_BODY()

public:
	UItemInfo();

};

USTRUCT (BlueprintType)
struct FWeaponInfoStruct
{
	GENERATED_USTRUCT_BODY()

public:

	//To set weapon based on weapon DT (Look at BaseMeleeWeapon header __ FWeaponStats)
	//TODO Add more rows in weapon level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats", meta = (AllowPrivateAccess = "true"))
		EWeaponLevel WeaponLevel;

	//Base weapon damage 
	//__effected by WeaponLevel
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats")
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
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats", meta = (ClampMin = 0.005f, ClampMax = 0.1f))
		float DurabilityLossRate;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon FX")
		UParticleSystem* DTImpactParticles;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon FX")
		UParticleSystem* DTUltAbilityParticles;
		
};

class AItem;

USTRUCT(BlueprintType)
struct FItemInfoStruct
{
	GENERATED_USTRUCT_BODY()

public:
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		USkeletalMesh* ItemSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UStaticMesh* ITemStaticMesh;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		//USkeletalMeshComponent* ItemSkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		EItemClass ItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AItem> ItemSubClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FString ItemDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", MultiLine = "true"))
		FString ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FString UseActionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UTexture2D* ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
		float ItemWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		bool bCanBeStacked;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		uint8 ItemCurrentStack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		uint8 ItemMaxStack;
	
	//-1 if not inside an inventory (Default state!)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 InventorySlotIndex;

	//Used to set certain aspects of the item's capabilities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 ItemLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		ELootTier LootTier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FWeaponInfoStruct WeaponInfoStruct;
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

