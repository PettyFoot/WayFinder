// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class WAYFINDER_API ItemInfoEnums
{
public:
	ItemInfoEnums();
	~ItemInfoEnums();
};

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

/////
///Weapons
/////

UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
	WT_Melee UMETA(DisplayName = "Melee"),
	WT_Ranged UMETA(DisplayName = "Ranged"),
	WT_Default  UMETA(DisplayName = "Default")

};
UENUM(BlueprintType)
enum class EMeleeType : uint8
{
	MT_Sword UMETA(DisplayName = "Sword"),
	MT_Axe UMETA(DisplayName = "Axe"),
	MT_Mace UMETA(DisplayName = "Mace"),
	MT_Default  UMETA(DisplayName = "Default")

};

UENUM(BlueprintType)
enum class ERangedType : uint8
{
	RT_Bow UMETA(DisplayName = "Bow"),
	RT_Hurlbat UMETA(DisplayName = "Hurlbat"),
	RT_Staff UMETA(DisplayName = "Staff"),
	RT_Default  UMETA(DisplayName = "Default")

};

/////
///Consumables
/////

UENUM(BlueprintType)
enum class EConsumableEffectType : uint8
{

	CET_Potion UMETA(DisplayName = "Potion"),
	CET_FoodDrink UMETA(DisplayName = "FoodDrink"),
	CET_Buff UMETA(DisplayName = "Buff"),
	CET_Default UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EPotionType : uint8
{
	PT_Heal UMETA(DisplayName = "Heal"),
	PT_Ult UMETA(DisplayName = "Ult"),
	PT_Stamina UMETA(DisplayName = "Stamina"),
	PT_Magica UMETA(DisplayName = "Magica"),
	PT_Poison UMETA(DisplayName = "Poison"),
	PT_Fire UMETA(DisplayName = "Fire"),
	PT_Bleed  UMETA(DisplayName = "Bleed"),
	PT_Default  UMETA(DisplayName = "Default")

};

UENUM(BlueprintType)
enum class EFoodDrinkType : uint8
{
	FD_Food UMETA(DisplayName = "Food"),
	FD_Drink UMETA(DisplayName = "Drink"),
	FD_Default  UMETA(DisplayName = "Default")
};


UENUM(BlueprintType)
enum class EBuffType : uint8
{

	BUFF_Fortify UMETA(DisplayName = "Fortify"), //Incease Health
	BUFF_Swift UMETA(DisplayName = "Speed"), //Increase speed (this is a constant never to be set from loot spawn table randomness)
	BUFF_Enrage UMETA(DisplayName = "Damage"), //Increase damage
	BUFF_Invigorate UMETA(DisplayName = "Invigorate"), //Increase ult charge rate (this should be adjusted per item level, but only random item tier)
	BUFF_Shield UMETA(DisplayName = "Shield"),//Increase Shield//Increase Shield
	BUFF_Default UMETA(DisplayName = "Default") //No buff effect, make sure buff enum effect is set to enable effect!
};



/////
///Armor
/////
UENUM(BlueprintType)
enum class EArmorType : uint8
{
	AT_Head UMETA(DisplayName = "Head"),
	AT_Torso UMETA(DisplayName = "Torso"),
	AT_Hands UMETA(DisplayName = "Hands"),
	AT_Legs UMETA(DisplayName = "Legs"),
	AT_Feet UMETA(DisplayName = "Feet"),
	AT_Default  UMETA(DisplayName = "Default")

};

UENUM(BlueprintType)
enum class EArmorClass : uint8
{
	AC_Light UMETA(DisplayName = "Light"),
	AC_Medium UMETA(DisplayName = "Medium"),
	AC_Heavy UMETA(DisplayName = "Heavy"),
	AC_Default  UMETA(DisplayName = "Default")

};



//////////////////////////////////////////////////

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



